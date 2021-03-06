TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    ../asterisk/apps/app_zapateller.c \
    ../asterisk/apps/app_while.c \
    ../asterisk/apps/app_waitforsilence.c \
    ../asterisk/apps/app_waitforring.c \
    ../asterisk/apps/app_voicemail.c \
    ../asterisk/apps/app_verbose.c \
    ../asterisk/apps/app_userevent.c \
    ../asterisk/apps/app_url.c \
    ../asterisk/apps/app_transfer.c \
    ../asterisk/apps/app_test.c \
    ../asterisk/apps/app_talkdetect.c \
    ../asterisk/apps/app_system.c \
    ../asterisk/apps/app_stack.c \
    ../asterisk/apps/app_speech_utils.c \
    ../asterisk/apps/app_softhangup.c \
    ../asterisk/apps/app_sms.c \
    ../asterisk/apps/app_skel.c \
    ../asterisk/apps/app_settransfercapability.c \
    ../asterisk/apps/app_setcdruserfield.c \
    ../asterisk/apps/app_setcallerid.c \
    ../asterisk/apps/app_sendtext.c \
    ../asterisk/apps/app_senddtmf.c \
    ../asterisk/apps/app_sayunixtime.c \
    ../asterisk/apps/app_rpt.c \
    ../asterisk/apps/app_record.c \
    ../asterisk/apps/app_realtime.c \
    ../asterisk/apps/app_readfile.c \
    ../asterisk/apps/app_read.c \
    ../asterisk/apps/app_random.c \
    ../asterisk/apps/app_radbridge.c \
    ../asterisk/apps/app_queue.c \
    ../asterisk/apps/app_privacy.c \
    ../asterisk/apps/app_playback.c \
    ../asterisk/apps/app_parkandannounce.c \
    ../asterisk/apps/app_page.c \
    ../asterisk/apps/app_osplookup.c \
    ../asterisk/apps/app_nbscat.c \
    ../asterisk/apps/app_mp3.c \
    ../asterisk/apps/app_morsecode.c \
    ../asterisk/apps/app_mixmonitor.c \
    ../asterisk/apps/app_milliwatt.c \
    ../asterisk/apps/app_meetme.c \
    ../asterisk/apps/app_macro.c \
    ../asterisk/apps/app_lookupcidname.c \
    ../asterisk/apps/app_lookupblacklist.c \
    ../asterisk/apps/app_ivrdemo.c \
    ../asterisk/apps/app_image.c \
    ../asterisk/apps/app_ices.c \
    ../asterisk/apps/app_hasnewvoicemail.c \
    ../asterisk/apps/app_gps.c \
    ../asterisk/apps/app_getcpeid.c \
    ../asterisk/apps/app_forkcdr.c \
    ../asterisk/apps/app_followme.c \
    ../asterisk/apps/app_flash.c \
    ../asterisk/apps/app_festival.c \
    ../asterisk/apps/app_externalivr.c \
    ../asterisk/apps/app_exec.c \
    ../asterisk/apps/app_echo.c \
    ../asterisk/apps/app_dumpchan.c \
    ../asterisk/apps/app_disa.c \
    ../asterisk/apps/app_directory.c \
    ../asterisk/apps/app_directed_pickup.c \
    ../asterisk/apps/app_dictate.c \
    ../asterisk/apps/app_dial.c \
    ../asterisk/apps/app_db.c \
    ../asterisk/apps/app_dahdiscan.c \
    ../asterisk/apps/app_dahdiras.c \
    ../asterisk/apps/app_dahdibarge.c \
    ../asterisk/apps/app_controlplayback.c \
    ../asterisk/apps/app_chanspy.c \
    ../asterisk/apps/app_channelredirect.c \
    ../asterisk/apps/app_chanisavail.c \
    ../asterisk/apps/app_cdr.c \
    ../asterisk/apps/app_authenticate.c \
    ../asterisk/apps/app_amd.c \
    ../asterisk/apps/app_alarmreceiver.c \
    ../asterisk/apps/app_adsiprog.c \
    ../asterisk/channels/pocsag.c \
    ../asterisk/channels/misdn_config.c \
    ../asterisk/channels/iax2-provision.c \
    ../asterisk/channels/iax2-parser.c \
    ../asterisk/channels/gentone.c \
    ../asterisk/channels/gentone-ulaw.c \
    ../asterisk/channels/chan_vpb.cc \
    ../asterisk/channels/chan_voter.c \
    ../asterisk/channels/chan_usrp.c \
    ../asterisk/channels/chan_usbradio.c \
    ../asterisk/channels/chan_tlb.c \
    ../asterisk/channels/chan_skinny.c \
    ../asterisk/channels/chan_sip.c \
    ../asterisk/channels/chan_simpleusb.c \
    ../asterisk/channels/chan_rtpdir.c \
    ../asterisk/channels/chan_phone.c \
    ../asterisk/channels/chan_oss.c \
    ../asterisk/channels/chan_nbs.c \
    ../asterisk/channels/chan_misdn.c \
    ../asterisk/channels/chan_mgcp.c \
    ../asterisk/channels/chan_local.c \
    ../asterisk/channels/chan_iax2.c \
    ../asterisk/channels/chan_h323.c \
    ../asterisk/channels/chan_gtalk.c \
    ../asterisk/channels/chan_features.c \
    ../asterisk/channels/chan_echolink.c \
    ../asterisk/channels/chan_dahdi.c \
    ../asterisk/channels/chan_beagle.c \
    ../asterisk/channels/chan_alsa.c \
    ../asterisk/channels/chan_agent.c \
    ../asterisk/main/utils.c \
    ../asterisk/main/ulaw.c \
    ../asterisk/main/udptl.c \
    ../asterisk/main/translate.c \
    ../asterisk/main/threadstorage.c \
    ../asterisk/main/term.c \
    ../asterisk/main/tdd.c \
    ../asterisk/main/strcompat.c \
    ../asterisk/main/srv.c \
    ../asterisk/main/slinfactory.c \
    ../asterisk/main/sha1.c \
    ../asterisk/main/sched.c \
    ../asterisk/main/say.c \
    ../asterisk/main/rtp.c \
    ../asterisk/main/privacy.c \
    ../asterisk/main/poll.c \
    ../asterisk/main/plc.c \
    ../asterisk/main/pbx.c \
    ../asterisk/main/netsock.c \
    ../asterisk/main/md5.c \
    ../asterisk/main/manager.c \
    ../asterisk/main/logger.c \
    ../asterisk/main/loader.c \
    ../asterisk/main/jitterbuf.c \
    ../asterisk/main/io.c \
    ../asterisk/main/indications.c \
    ../asterisk/main/image.c \
    ../asterisk/main/http.c \
    ../asterisk/main/global_datastores.c \
    ../asterisk/main/fskmodem.c \
    ../asterisk/main/frame.c \
    ../asterisk/main/fixedjitterbuf.c \
    ../asterisk/main/file.c \
    ../asterisk/main/enum.c \
    ../asterisk/main/dsp.c \
    ../asterisk/main/dnsmgr.c \
    ../asterisk/main/dns.c \
    ../asterisk/main/dial.c \
    ../asterisk/main/devicestate.c \
    ../asterisk/main/db.c \
    ../asterisk/main/cryptostub.c \
    ../asterisk/main/config.c \
    ../asterisk/main/cli.c \
    ../asterisk/main/chanvars.c \
    ../asterisk/main/channel.c \
    ../asterisk/main/cdr.c \
    ../asterisk/main/callerid.c \
    ../asterisk/main/buildinfo.c \
    ../asterisk/main/autoservice.c \
    ../asterisk/main/audiohook.c \
    ../asterisk/main/astobj2.c \
    ../asterisk/main/astmm.c \
    ../asterisk/main/asterisk.c \
    ../asterisk/main/ast_expr2f.c \
    ../asterisk/main/ast_expr2.c \
    ../asterisk/main/app.c \
    ../asterisk/main/alaw.c \
    ../asterisk/main/aestab.c \
    ../asterisk/main/aeskey.c \
    ../asterisk/main/aescrypt.c \
    ../asterisk/main/acl.c \
    ../asterisk/main/abstract_jb.c

OTHER_FILES += \
    ../asterisk/apps/Makefile \
    ../asterisk/apps/app_rpt.c-from-svn-0.325 \
    ../asterisk/apps/app_rpt.c-2015-03-15 \
    ../asterisk/apps/app_rpt.c-2014-08-24 \
    ../asterisk/channels/Makefile \
    ../asterisk/channels/chan_voter.c-pre-redundancy \
    ../asterisk/main/Makefile \
    ../asterisk/main/ast_expr2.y

HEADERS += \
    ../asterisk/apps/leave.h \
    ../asterisk/apps/enter.h \
    ../asterisk/channels/ring10.h \
    ../asterisk/channels/pocsag.h \
    ../asterisk/channels/iax2.h \
    ../asterisk/channels/iax2-provision.h \
    ../asterisk/channels/iax2-parser.h \
    ../asterisk/channels/DialTone.h \
    ../asterisk/channels/chan_usrp.h \
    ../asterisk/channels/chan_usbradio.c-from-svn-1517 \
    ../asterisk/channels/chan_usbradio.c-2015-03-14 \
    ../asterisk/channels/chan_simpleusb.c.save \
    ../asterisk/channels/chan_simpleusb.c-from-svn-1515 \
    ../asterisk/channels/answer.h \
    ../asterisk/main/fixedjitterbuf.h \
    ../asterisk/main/ecdisa.h \
    ../asterisk/main/coef_out.h \
    ../asterisk/main/coef_in.h \
    ../asterisk/main/ast_expr2.h \
    ../asterisk/main/ast_expr2.fl \
    ../asterisk/main/aesopt.h \
    ../asterisk/include/jitterbuf.h \
    ../asterisk/include/asterisk.h \
    ../asterisk/include/asterisk/utils.h \
    ../asterisk/include/asterisk/unaligned.h \
    ../asterisk/include/asterisk/ulaw.h \
    ../asterisk/include/asterisk/udptl.h \
    ../asterisk/include/asterisk/translate.h \
    ../asterisk/include/asterisk/transcap.h \
    ../asterisk/include/asterisk/tonezone_compat.h \
    ../asterisk/include/asterisk/time.h \
    ../asterisk/include/asterisk/threadstorage.h \
    ../asterisk/include/asterisk/term.h \
    ../asterisk/include/asterisk/tdd.h \
    ../asterisk/include/asterisk/strings.h \
    ../asterisk/include/asterisk/stringfields.h \
    ../asterisk/include/asterisk/srv.h \
    ../asterisk/include/asterisk/speech.h \
    ../asterisk/include/asterisk/smdi.h \
    ../asterisk/include/asterisk/slinfactory.h \
    ../asterisk/include/asterisk/sha1.h \
    ../asterisk/include/asterisk/sched.h \
    ../asterisk/include/asterisk/say.h \
    ../asterisk/include/asterisk/rtp.h \
    ../asterisk/include/asterisk/res_odbc.h \
    ../asterisk/include/asterisk/privacy.h \
    ../asterisk/include/asterisk/poll-compat.h \
    ../asterisk/include/asterisk/plc.h \
    ../asterisk/include/asterisk/pbx.h \
    ../asterisk/include/asterisk/paths.h \
    ../asterisk/include/asterisk/options.h \
    ../asterisk/include/asterisk/netsock.h \
    ../asterisk/include/asterisk/musiconhold.h \
    ../asterisk/include/asterisk/monitor.h \
    ../asterisk/include/asterisk/module.h \
    ../asterisk/include/asterisk/md5.h \
    ../asterisk/include/asterisk/manager.h \
    ../asterisk/include/asterisk/logger.h \
    ../asterisk/include/asterisk/lock.h \
    ../asterisk/include/asterisk/localtime.h \
    ../asterisk/include/asterisk/linkedlists.h \
    ../asterisk/include/asterisk/jingle.h \
    ../asterisk/include/asterisk/jabber.h \
    ../asterisk/include/asterisk/io.h \
    ../asterisk/include/asterisk/inline_api.h \
    ../asterisk/include/asterisk/indications.h \
    ../asterisk/include/asterisk/image.h \
    ../asterisk/include/asterisk/http.h \
    ../asterisk/include/asterisk/global_datastores.h \
    ../asterisk/include/asterisk/fskmodem.h \
    ../asterisk/include/asterisk/frame.h \
    ../asterisk/include/asterisk/file.h \
    ../asterisk/include/asterisk/features.h \
    ../asterisk/include/asterisk/enum.h \
    ../asterisk/include/asterisk/endian.h \
    ../asterisk/include/asterisk/dundi.h \
    ../asterisk/include/asterisk/dsp.h \
    ../asterisk/include/asterisk/doxyref.h \
    ../asterisk/include/asterisk/dnsmgr.h \
    ../asterisk/include/asterisk/dns.h \
    ../asterisk/include/asterisk/dial.h \
    ../asterisk/include/asterisk/devicestate.h \
    ../asterisk/include/asterisk/dahdi_compat.h \
    ../asterisk/include/asterisk/crypto.h \
    ../asterisk/include/asterisk/config.h \
    ../asterisk/include/asterisk/compiler.h \
    ../asterisk/include/asterisk/compat.h \
    ../asterisk/include/asterisk/cli.h \
    ../asterisk/include/asterisk/chanvars.h \
    ../asterisk/include/asterisk/channel.h \
    ../asterisk/include/asterisk/cdr.h \
    ../asterisk/include/asterisk/causes.h \
    ../asterisk/include/asterisk/callerid.h \
    ../asterisk/include/asterisk/buildopts.h \
    ../asterisk/include/asterisk/autoconfig.h.in \
    ../asterisk/include/asterisk/autoconfig.h \
    ../asterisk/include/asterisk/audiohook.h \
    ../asterisk/include/asterisk/astosp.h \
    ../asterisk/include/asterisk/astobj2.h \
    ../asterisk/include/asterisk/astobj.h \
    ../asterisk/include/asterisk/astmm.h \
    ../asterisk/include/asterisk/astdb.h \
    ../asterisk/include/asterisk/ast_expr.h \
    ../asterisk/include/asterisk/app.h \
    ../asterisk/include/asterisk/alaw.h \
    ../asterisk/include/asterisk/agi.h \
    ../asterisk/include/asterisk/aes.h \
    ../asterisk/include/asterisk/ael_structs.h \
    ../asterisk/include/asterisk/adsi.h \
    ../asterisk/include/asterisk/acl.h \
    ../asterisk/include/asterisk/abstract_jb.h

