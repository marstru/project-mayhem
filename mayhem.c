/* This file is part of wmdump
 * Copyright (c) 2011 Gianni Tedesco
 * Released under the terms of GPLv3
*/
#include <wmdump.h>
#include <wmvars.h>
#include <mayhem.h>
#include <os.h>

#include <rtmp/amf.h>
#include <rtmp/rtmp.h>

#include "cvars.h"

#define SWF_URL "http://www.naiadsystems.com/flash/generic/20111122/avchat.swf"

struct _mayhem {
	rtmp_t rtmp;
};

static int invoke_connect(struct _mayhem *m, struct _wmvars *v)
{
	invoke_t inv;
	amf_t obj;
	int ret;

	inv = amf_invoke_new(7);
	if ( NULL == inv )
		return 0;

	if ( !amf_invoke_append(inv, amf_string("connect")) )
		goto err;
	if ( !amf_invoke_append(inv, amf_number(1.0)) )
		goto err;

	obj = amf_object();
	if ( !amf_object_set(obj, "app", amf_stringf("reflect/%d", v->sid)) )
		goto err_obj;
	if ( !amf_object_set(obj, "flashVer", amf_string("LNX 11,1,102,55")) )
		goto err_obj;
	if ( !amf_object_set(obj, "swfUrl", amf_string(SWF_URL)) )
		goto err_obj;
	if ( !amf_object_set(obj, "tcUrl", amf_string(v->tcUrl)) )
		goto err_obj;
	if ( !amf_object_set(obj, "fpad", amf_bool(0)) )
		goto err_obj;
	if ( !amf_object_set(obj, "capabilities", amf_number(239.0)) )
		goto err_obj;
	if ( !amf_object_set(obj, "audioCodecs", amf_number(3575.0)) )
		goto err_obj;
	if ( !amf_object_set(obj, "videoCodecs", amf_number(252.0)) )
		goto err_obj;
	if ( !amf_object_set(obj, "videoFunction", amf_number(1.0)) )
		goto err_obj;
	if ( !amf_object_set(obj, "pageUrl", amf_string(v->pageurl)) )
		goto err_obj;
	if ( !amf_object_set(obj, "objectEncoding", amf_number(3.0)) )
		goto err_obj;
	if ( !amf_invoke_append(inv, obj) )
		goto err;

	if ( !amf_invoke_append(inv, amf_string("0")) )
		goto err;

	if ( !amf_invoke_append(inv, amf_stringf("%d", v->pid)) )
		goto err;

	if ( !amf_invoke_append(inv, amf_string(NULL) ))
		goto err;

	obj = amf_object();
	if ( !amf_object_set(obj, "sid", amf_stringf("%d", v->sid)) )
		goto err_obj;
	if ( !amf_object_set(obj, "ldmov", amf_string(v->ldmov)) )
		goto err_obj;
	if ( !amf_object_set(obj, "pid", amf_stringf("%d", v->pid)) )
		goto err_obj;
	if ( !amf_object_set(obj, "sessionType", amf_string(v->sessiontype)) )
		goto err_obj;
	if ( !amf_object_set(obj, "signupargs", amf_string(v->signupargs)) )
		goto err_obj;
	if ( !amf_object_set(obj, "g", amf_string(v->g)) )
		goto err_obj;
	if ( !amf_object_set(obj, "ft", amf_stringf("%d", v->ft)) )
		goto err_obj;
	if ( !amf_object_set(obj, "hd", amf_stringf("%d", v->hd)) )
		goto err_obj;
	if ( !amf_object_set(obj, "sk", amf_string(v->sk)) )
		goto err_obj;
	if ( !amf_object_set(obj, "nickname", amf_string(v->nickname)) )
		goto err_obj;
	if ( !amf_object_set(obj, "sakey", amf_string(v->sakey)) )
		goto err_obj;
	if ( !amf_object_set(obj, "lang", amf_string(v->lang)) )
		goto err_obj;
	if ( !amf_object_set(obj, "version", amf_number(7.0)) )
		goto err_obj;
	if ( !amf_object_set(obj, "srv", amf_stringf("%d", v->srv)) )
		goto err_obj;

	if ( !amf_invoke_append(inv, obj) )
		goto err;

	ret = rtmp_invoke(m->rtmp, inv);
	amf_invoke_free(inv);
	return ret;
err_obj:
	amf_free(obj);
err:
	amf_invoke_free(inv);
	return 0;
}

void mayhem_close(mayhem_t m)
{
	if ( m ) {
		rtmp_close(m->rtmp);
		free(m);
	}
}

mayhem_t mayhem_connect(wmvars_t vars)
{
	struct _mayhem *m;

	m = calloc(1, sizeof(*m));
	if ( NULL == m )
		goto out;

	m->rtmp = rtmp_connect(vars->tcUrl);
	if ( NULL == m->rtmp )
		goto out_free;

	if ( !invoke_connect(m, vars) )
		goto out_free;

	while ( rtmp_pump(m->rtmp) )
		/* do nothing */;

	/* success */
	goto out;
out_free:
	free(m);
	m = NULL;
out:
	return m;
}