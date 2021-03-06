/**
 * \file ker_channel.c
 *
 * Channel creation and manipulation routines.
 *
 * \copyright 2007, QNX Software Systems. All Rights Reserved.
 *
 * 64-bit conversion (c) 2021 Yuri Zaporozhets <r_tty@yahoo.co.uk>
 *
 * \license QNX NCEULA 1.01
 *          http://www.qnx.com/legal/licensing/dev_license/eula/nceula1_01.html
 */

#include "externs.h"

/**
 * \brief Create a channel.
 */
int kdecl ker_channel_create(tThread * act, struct kerargs_channel_create *kap)
{
    tProcess *prp;
    tChannel *chp;
    int chid;
    tSoul *souls;
    tVector *chvec;

    prp = act->process;
    if ((kap->flags & (QRV_FLG_CHF_THREAD_DEATH | QRV_FLG_CHF_COID_DISCONNECT)) && prp->death_chp) {
        return EBUSY;
    }

    if (kap->flags & (QRV_FLG_CHF_ASYNC | QRV_FLG_CHF_GLOBAL)) {
#ifdef CONFIG_ASYNC_MSG
        souls = &chasync_souls;
#endif
        if (kap->event != NULL) {
            RD_PROBE_INT(act, kap->event, sizeof(*kap->event) / sizeof(int));
        }
    }

    if (kap->flags & QRV_FLG_CHF_GLOBAL) {
        prp = NULL;
        souls = &chgbl_souls;
        chvec = &chgbl_vector;
        if (kap->cred != NULL) {
            RD_PROBE_INT(act, kap->cred, sizeof(*kap->cred) / sizeof(int));
        }
    } else {
#ifndef CONFIG_ASYNC_MSG
        souls = &channel_souls;
#endif
        chvec = &prp->chancons;
    }

    lock_kernel();
    // Allocate a channel entry.
    if ((chp = object_alloc(prp, souls)) == NULL) {
        return EAGAIN;
    }

    chp->type = TYPE_CHANNEL;
    chp->process = prp;
    chp->flags = kap->flags;

    if (kap->flags & QRV_FLG_CHF_GLOBAL) {
        if (chp->reply_queue)
            crash();
    }
    // Check if this is the network managers msg channel.
    // Don't do this until we know the channel's fully created, so
    // we (mostly) don't have to worry about net.* fields not getting cleared
    // on an error.
    if (kap->flags & QRV_FLG_CHF_NET_MSG) {
        if (!kerisroot(act)) {
            object_free(prp, &channel_souls, chp);
            return EPERM;
        }
        if (net.prp || net.chp) {
            object_free(prp, &channel_souls, chp);
            return EAGAIN;
        }
        net.prp = prp;
        net.chp = chp;
    }
    // Add the channel to the channels vector.
    chid = vector_add(chvec, chp, 1);
    if (chid == -1) {
        object_free(prp, &channel_souls, chp);
        if (kap->flags & QRV_FLG_CHF_NET_MSG) {
            net.prp = NULL;
            net.chp = NULL;
        }
        return EAGAIN;
    }

    if (kap->flags & (QRV_FLG_CHF_THREAD_DEATH | QRV_FLG_CHF_COID_DISCONNECT)) {
        if (prp) {              /* not global channel */
            prp->death_chp = chp;
        }
    }

    if (kap->flags & QRV_FLG_CHF_GLOBAL) {
        tChannelGbl *chgbl = (tChannelGbl *) chp;

        SET_GLOBAL_CHAN(chid);
        chgbl->event.sigev_notify = SIGEV_NONE;
        chgbl->max_num_buffer = kap->maxbuf;
        chgbl->buffer_size = kap->bufsize;
        if (kap->event != NULL) {
            memcpy(&chgbl->event, kap->event, sizeof(*kap->event));
            chgbl->ev_prp = act->process;
        }
        if (kap->cred) {
            memcpy(&chgbl->cred, kap->cred, sizeof(chgbl->cred));
        }
    }

#ifdef CONFIG_ASYNC_MSG
    if (kap->flags & QRV_FLG_CHF_ASYNC) {
        ((tChannelAsync *) chp)->event.sigev_notify = SIGEV_NONE;
        if (kap->event != NULL) {
            memcpy(&((tChannelAsync *) chp)->event, kap->event, sizeof(*kap->event));
            ((tChannelAsync *) chp)->ev_prp = act->process;
        }
    }
#endif

    chp->chid = chid;

    SETKSTATUS(act, chid);
    return ENOERROR;
}


#if defined(VARIANT_smp) && defined(SMP_MSGOPT)
static void clear_msgxfer(tThread * thp)
{

    switch (thp->state) {
    case STATE_REPLY:
        thp->flags &= ~QRV_FLG_THR_UNBLOCK_REQ;
        /* fall through */

    case STATE_SEND:{
            tConnect *cop = thp->blocked_on;
            if (--cop->links == 0) {
                connect_detach(cop, thp->priority);
            }
        }

        /* fall through */
    case STATE_RECEIVE:
        thp->internal_flags &= ~QRV_FLG_ITF_RCVPULSE;
        thp->restart = NULL;
        break;
    default:
#ifndef NDEBUG
        kprintf("\nclear_msgxfer: should not reach here. thread state %x\n", thp->state);
#endif
        crash();
        /* NOTREACHED */
        return;
    }
    if (thp->state == STATE_SEND) {
        pril_rem(&((tChannel *) thp->blocked_on)->send_queue, thp);
    } else {
        LINKPRIL_REM(thp);
    }

    thp->state = STATE_READY;
}

#define CLR_MSGXFER(thp) if((thp)->internal_flags & QRV_FLG_ITF_MSG_DELIVERY) clear_msgxfer(thp)
#else
#define CLR_MSGXFER(thp)
#endif


int kdecl ker_channel_destroy(tThread * act, struct kerargs_channel_destroy *kap)
{
    tProcess *prp;
    tChannel *chp;
    tConnect *cop;
    tThread *thp;
    int i, chid;
    tSoul *souls;
    tVector *chvec;

    prp = act->process;

    chid = kap->chid;
    chvec = &prp->chancons;
    if (IS_GLOBAL_CHAN_SET(chid)) {
        CLEAR_GLOBAL_CHAN(chid);
        chvec = &chgbl_vector;
    }
    // Make sure the channel is valid.
    chp = vector_lookup2(chvec, chid);
    if ((chp == NULL) || (chp->type != TYPE_CHANNEL)) {
        return EINVAL;
    }

    lock_kernel();

    // Mark channel as unavailable.
    vector_flag(chvec, chid, 1);
    // Check if the network manager is releasing his message channel.
    if ((prp == net.prp) && (chp == net.chp)) {
        static unsigned retries;

        for (i = 0; i < vthread_vector.nentries; ++i) {
            tVthread *vthp;

            vthp = vector_rem(&vthread_vector, i);
            if (vthp != NULL) {
                unsigned saved_flags;
                saved_flags = vthp->flags;
                vthp->flags &= ~QRV_FLG_THR_KILLSELF;
                if (!force_ready((tThread *) (void *) vthp, EINTR | _FORCE_SET_ERROR)) {
                    // We couldn't force_ready the thread, we must
                    // have been in the middle of a SMP_MSGOPT message
                    // pass. The force_ready() call would have invoked
                    // force_ready_msgxfer() to abort the transfer, so let's
                    // get out of the kernel and let that be processed.
                    // If we restart 1 million times without getting out
                    // of here, something's gone wrong....
                    if (++retries == 1000000)
                        crash();
                    vthp->flags = saved_flags;
                    vector_add(&vthread_vector, vthp, i);
                    KERCALL_RESTART(act);
                    return ENOERROR;
                }
                object_free(NULL, &vthread_souls, vthp);
            }
            KER_PREEMPT(act, ENOERROR);
        }
        retries = 0;
        net.prp = NULL;
        net.chp = NULL;
    }
    // Check for destruction of the death channel.
    if (chp == prp->death_chp) {
        prp->death_chp = NULL;
    }
    // Unblock all threads send blocked on the channel
    // We have to do this before we NULL the cop->channel pointers
    // below so that force_ready can find the appropriate PRIL_HEAD structure.
    while ((thp = pril_first(&chp->send_queue))) {
        switch (TYPE_MASK(thp->type)) {
        case TYPE_PULSE:
        case TYPE_VPULSE:
            pril_rem(&chp->send_queue, thp);
            object_free(chp->process, &pulse_souls, thp);
            break;
        case TYPE_VTHREAD:
            // Turn on QRV_FLG_THR_KERERR_SET so that if net_send2() calls
            // kererr(), it won't advance the IP - we're going to be
            // restarting the kernel call so that qnet has a chance
            // to process the pulse
            act->flags |= QRV_FLG_THR_KERERR_SET;
            (void) net_send2((tKerArgs *) (void *) kap, thp->un.net.vtid, thp->blocked_on, thp);
            CLR_MSGXFER(thp);
            KERCALL_RESTART(act);
            return ENOERROR;
        case TYPE_THREAD:
            force_ready(thp, ESRCH);
            CLR_MSGXFER(thp);
            break;
        default:
            crash();
            break;
        }
        KER_PREEMPT(act, ENOERROR);
    }

    // Unblock all threads receive blocked on the channel
    while ((thp = chp->receive_queue)) {
        CRASHCHECK((TYPE_MASK(thp->type) != TYPE_THREAD) && (TYPE_MASK(thp->type) != TYPE_VTHREAD));
        force_ready(thp, ESRCH);
        CLR_MSGXFER(thp);
        KER_PREEMPT(act, ENOERROR);
    }

    if (!(chp->flags & (QRV_FLG_CHF_ASYNC | QRV_FLG_CHF_GLOBAL))) {
        // Unblock all threads reply blocked on the channel
        while ((thp = chp->reply_queue)) {
            switch (TYPE_MASK(thp->type)) {
            case TYPE_VTHREAD:
                // Turn on QRV_FLG_THR_KERERR_SET so that if net_send2() calls
                // kererr(), it won't advance the IP - we're going to be
                // restarting the kernel call so that qnet has a chance
                // to process the pulse
                act->flags |= QRV_FLG_THR_KERERR_SET;
                (void) net_send2((tKerArgs *) (void *) kap, thp->un.net.vtid, thp->blocked_on, thp);
                CLR_MSGXFER(thp);
                KERCALL_RESTART(act);
                return ENOERROR;
            case TYPE_THREAD:
                // Disable unblock pulse since we don't want to add more stuff
                // to this channel that we're in the process of destroying...
                force_ready(thp, ESRCH | _FORCE_SET_ERROR | _FORCE_NO_UNBLOCK);
                CLR_MSGXFER(thp);
                break;
            default:
                crash();
            }
            KER_PREEMPT(act, ENOERROR);
        }
    } else {
        // release all packages/send requests
        if (chp->flags & QRV_FLG_CHF_GLOBAL) {
            struct gblmsg_entry *gblmsg;
            tChannelGbl *chgbl = (tChannelGbl *) chp;

            if (chgbl->free != NULL) {
                _sfree(chgbl->free, chgbl->buffer_size + sizeof(*gblmsg));
            }
            while ((gblmsg = (struct gblmsg_entry *) chp->reply_queue)) {
                chp->reply_queue = (void *) gblmsg->next;
                _sfree(gblmsg, chgbl->buffer_size + sizeof(*gblmsg));

            }
        }
    }

    // Find all server connections and mark their channels as NULL.
    if (!(chp->flags & QRV_FLG_CHF_GLOBAL)) {
        for (i = 0; i < prp->chancons.nentries; ++i) {
            if ((cop = VECP2(cop, &prp->chancons, i))) {
                if (cop->type == TYPE_CONNECTION && cop->channel == chp && cop->scoid == i) {

                    // Remove the server connection from the servers channels vector.
                    vector_rem(&prp->chancons, i);

                    if (cop->links == 0) {
                        // Remove server connection object.
                        object_free(prp, &connect_souls, cop);
                    } else {
                        do {
                            if (cop->process && cop->process->death_chp) {
                                connect_coid_disconnect(cop, cop->process->death_chp,
                                                        act->priority);
                            }
                            // Null channel pointer since it is gone.
                            cop->channel = NULL;
                        } while ((cop = cop->next) && (!(chp->flags & QRV_FLG_CHF_ASYNC)));
                    }
                }
            }
            KER_PREEMPT(act, ENOERROR);
        }
    } else {
        cop = ((tChannelGbl *) chp)->cons;
        while (cop) {
            tConnect *next = cop->next;
            cop->channel = NULL;
            cop->next = NULL;
            cop = next;
        }
    }

    // Remove the channel from the servers channels vector.
    if ((chp = vector_rem(chvec, chid)) == NULL) {
        return EINVAL;
    }

    if (chp->flags & QRV_FLG_CHF_GLOBAL)
        souls = &chgbl_souls;
    else
        souls = &channel_souls;
#ifdef CONFIG_ASYNC_MSG
    if (chp->flags & QRV_FLG_CHF_ASYNC)
        souls = &chasync_souls;
#endif
    // Release the channel entry.
    object_free(prp, souls, chp);

    return EOK;
}
