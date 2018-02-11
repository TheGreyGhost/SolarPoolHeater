
var _elqQ=_elqQ||[];_elqQ.push(['elqSetSiteId','248385305']);_elqQ.push(['elqTrackPageView']);(function(){function async_load(){var s=document.createElement('script');s.type='text/javascript';s.async=true;s.src='//img.en25.com/i/elqCfg.min.js';var x=document.getElementsByTagName('script')[0];x.parentNode.insertBefore(s,x);}
if(window.addEventListener)window.addEventListener('DOMContentLoaded',async_load,false);else if(window.attachEvent)window.attachEvent('onload',async_load);})();


function MaxFire_Agent_Manager(args)
{this.tc=0;this.vc=0;this.type='';this.full_version='';this.version='';if(args){for(key in args){this[key]=args[key];}}
return(this);}
MaxFire_Agent_Manager.prototype.prep=function()
{return(this);}
MaxFire_Agent_Manager.prototype.compute=function()
{if(!this.tc){this.compute_type();}
if(!this.vc){this.compute_version();}}
MaxFire_Agent_Manager.prototype.compute_type=function()
{this.type='';if(this.matches_msie()){this.type='msie';}
else if(this.matches_firefox()){this.type='firefox';}
else if(this.matches_chrome()){this.type='chrome';}
else if(this.matches_opera()){this.type='opera';}
this.tc=1;}
MaxFire_Agent_Manager.prototype.compute_version=function()
{this.full_version='';this.version='';if(this.type=='msie'){var hit=navigator.userAgent.match(/MSIE ([^;]+);/i);if(hit){this.full_version=RegExp.$1;}}
else if(this.type=='firefox'){var hit=navigator.userAgent.match(/Firefox\/([0-9\.]+)/i);if(hit){this.full_version=RegExp.$1;}}
else if(this.type=='chrome'){var hit=navigator.userAgent.match(/Chrome\/([0-9\.]+)/i);if(hit){this.full_version=RegExp.$1;}}
else if(this.type=='opera'){var hit=navigator.userAgent.match(/Opera\/([0-9\.]+)/i);if(hit){this.full_version=RegExp.$1;}}
if(this.full_version){var hit=this.full_version.match(/^(\d+)/);if(hit){this.version=RegExp.$1;}}
this.vc=1;}
MaxFire_Agent_Manager.prototype.is_msie6_relic=function()
{this.compute();return(this.type=='msie'&&this.version<7);}
MaxFire_Agent_Manager.prototype.is_msie=function()
{this.compute();return(this.type=='msie');}
MaxFire_Agent_Manager.prototype.is_firefox=function()
{this.compute();return(this.type=='firefox');}
MaxFire_Agent_Manager.prototype.is_chrome=function()
{this.compute();return(this.type=='chrome');}
MaxFire_Agent_Manager.prototype.is_opera=function()
{this.compute();return(this.type=='opera');}
MaxFire_Agent_Manager.prototype.matches_msie=function()
{return(navigator.appName=="Microsoft Internet Explorer");}
MaxFire_Agent_Manager.prototype.matches_firefox=function()
{return(navigator.appName=="Netscape"&&navigator.userAgent.match(/Firefox\/\d+/));}
MaxFire_Agent_Manager.prototype.matches_chrome=function()
{return(navigator.appName=="Netscape"&&navigator.userAgent.match(/Chrome\/\d+/));}
MaxFire_Agent_Manager.prototype.matches_opera=function()
{return(navigator.appName=="Opera");}


function MaxFire_Ajax_Manager()
{this.success=1;this.error_msg="";this.mode="get";this.action_id="generic";this.stags=[];return(this);}
MaxFire_Ajax_Manager.prototype.set_error=function(msg)
{this.error_msg="ERROR "+this.action_id+": "+msg;this.success=0;}
MaxFire_Ajax_Manager.prototype.load_xcontent=function(cb_func_str,url,params)
{var mid=params.mid;if(mid==undefined||mid==-1){mid=params.mid=Mary[1];}
Ajax.init_xget({obj:0,action_id:"load_xcontent",handler:cb_func_str,'url':url,'params':params});Ajax.run(1);return(1);}
MaxFire_Ajax_Manager.prototype.init_xget=function(wsi)
{this.init_get(wsi);this.mode='xget';this.url+='&jsonp='+wsi.handler;return(1);}
MaxFire_Ajax_Manager.prototype.init_get=function(wsi)
{this.wsi=wsi;this.prepare_get(wsi.obj,wsi.action_id,wsi.url,wsi.handler,wsi.params);}
MaxFire_Ajax_Manager.prototype.prepare_get=function(obj,action_id,url,handler,params)
{this.mode="get";this.obj=obj;this.action_id=action_id;this.url=url;this.handler=handler;this.params=params||0;if(params){var param_str=this.build_param_string(params);var delim=url.indexOf("?")>=0?'&':'?';url+=delim+param_str;this.url=url;}
this.success=1;this.error_msg="";this.init_req(handler);if(this.req==null){this.set_error("No Ajax Support");return(0);}
return(1);}
MaxFire_Ajax_Manager.prototype.init_post=function(wsi)
{this.wsi=wsi;this.prepare_post(wsi.obj,wsi.action_id,wsi.url,wsi.handler,wsi.params);}
MaxFire_Ajax_Manager.prototype.prepare_post=function(obj,action_id,url,handler,params)
{this.mode="post";this.obj=obj;this.action_id=action_id;this.url=url;this.handler=handler;this.params=params||0;this.success=1;this.error_msg="";this.init_req(handler);if(this.req==null){this.set_error("No Ajax Support");return(0);}
return(1);}
MaxFire_Ajax_Manager.prototype.run=function(safe)
{if(safe){this.add_maxcred();}
var method="run_"+this.mode;return(this[method]());}
MaxFire_Ajax_Manager.prototype.run_get=function()
{this.req.open("GET",this.url,true);this.req.send(null);}
MaxFire_Ajax_Manager.prototype.run_post=function()
{var ctype="application/x-www-form-urlencoded";var param_str=this.build_param_string(this.params);this.req.open("POST",this.url,true);this.req.setRequestHeader("Content-Type",ctype);this.req.setRequestHeader("Content-Length",param_str.length);this.req.setRequestHeader("Connection","close");this.req.send(param_str);}
MaxFire_Ajax_Manager.prototype.run_xget=function()
{var head_tag=document.getElementsByTagName("head")[0];var script_tag=document.createElement('script');script_tag.type='text/javascript';script_tag.src=this.url;this.stags.push(script_tag);head_tag.appendChild(script_tag);}
MaxFire_Ajax_Manager.prototype.add_maxcred=function()
{var maxcred=this.get_maxcred();if(this.mode=="post"){this.params.maxcred=maxcred;}
else{var delim=this.url.indexOf("?")>=0?'&':'?';this.url+=delim+"maxcred="+escape(maxcred);}}
MaxFire_Ajax_Manager.prototype.get_maxcred=function()
{var s=document.cookie;var f=s.match(/(^|;)\s*MAX_ID_TICKET=([^;\s]+)/);var val=unescape(RegExp.$2);return(val);}
MaxFire_Ajax_Manager.prototype.build_param_string=function(params)
{var pairs=[];for(key in params){pairs.push(key+"="+escape(params[key]));}
return(pairs.join("&"));}
MaxFire_Ajax_Manager.prototype.init_req=function(handler)
{var req=null;if(window.XMLHttpRequest){req=new XMLHttpRequest();}
else if(window.ActiveXObject){req=new ActiveXObject("Microsoft.XMLHTTP");}
this.req=req;if(req&&handler){this.req.onreadystatechange=handler;}
return(req);}
MaxFire_Ajax_Manager.prototype.is_done=function()
{return(this.req.readyState==4||this.req.readyState=="complete");}
MaxFire_Ajax_Manager.prototype.process_json_response=function()
{var wrapper=0;this.wrapper=0;eval(this.req.responseText);if(!wrapper){this.set_error("Bad wrapper");return(0);}
this.wrapper=wrapper;if(!wrapper.success){this.set_error(wrapper.msg);}
return(wrapper.mdata);}
MaxFire_Ajax_Manager.prototype.debug_json_response=function()
{alert(this.req.responseText);}
MaxFire_Ajax_Manager.prototype.report_error=function(main_msg)
{var msg1="ERROR: "+main_msg+"\n\n";var msg2=this.wrapper.msg+"\n\nDetails:\n"+this.wrapper.details;alert(msg1+msg2);return("");}
MaxFire_Ajax_Manager.prototype.blip_feedback=function(elem_id,ack_msg)
{var elem=document.getElementById(elem_id);this.feedback_elem=elem;this.feedback_elem_id=elem_id;if(!elem){alert(ack_msg);}
else{elem.innerHTML=ack_msg;this.feedback_timeout=setTimeout("Ajax.clear_feedback()",1500);}}
MaxFire_Ajax_Manager.prototype.clear_feedback=function()
{var elem=document.getElementById(this.feedback_elem_id);if(elem){elem.innerHTML="";}}


function MaxFire_Drawer_Manager(args)
{if(!args.eid){return(false);}
var elem=dyna.elem(args.eid);if(!elem){return(false);}
elem.max_obj=this;++dyna.icount;this.eid=args.eid;this.name=args.eid;this.elem=elem;this.state='in';this.anim_dir='down';this.anim_field='height';this.run_time=250;this.run_tick=10;for(key in args){this[key]=args[key];}
if(!this.vname){this.vname=this.name;}
this.cb_func=this.vname+".avar_tick()";this.avar=new MaxFire_Dyna_SimpleAvar({run_time:this.run_time,run_tick:this.run_tick,cb_func:this.cb_func,step_def:5,dom_refresh:true,dom_auto:true,dom_elem:elem,dom_field:this.anim_field});this.min_val=0;this.max_val=parseInt(elem.style[this.anim_field]);elem.style.display="none";elem.style[this.anim_field]="0px";return(this);}
MaxFire_Drawer_Manager.prototype.slide_toggle=function(src_ref)
{var rv=0;if(this.state=='in'){rv=this.slide_out(src_ref);}
else if(this.state=='out'){rv=this.slide_in(src_ref);}
else if(this.state=='anim'){rv=0;}
return(rv);}
MaxFire_Drawer_Manager.prototype.slide_out=function(src_ref)
{this.src_ref=src_ref;if(this.state!='in'){return(false);}
this.goal='out';this.state='anim';this.avar.prepare(this.min_val,this.max_val);this.avar.begin();return(1);}
MaxFire_Drawer_Manager.prototype.slide_in=function(src_ref)
{this.src_ref=src_ref;if(this.state!='out'){return(false);}
this.goal='in';this.state='anim';this.avar.prepare(this.max_val,this.min_val);this.avar.begin();return(1);}
MaxFire_Drawer_Manager.prototype.avar_tick=function()
{this.avar.monitor();if(this.avar.not_done){this.avar.update();return(0);}
this.avar.end();this.state=this.goal;this.goal='';}


function MaxFire_Dyna_Manager(args)
{this.icount=0;if(args){for(key in args){this[key]=args[key];}}
return(this);}
MaxFire_Dyna_Manager.prototype.elem=function(eid)
{return(document.getElementById(eid));}
MaxFire_Dyna_Manager.prototype.toggle_elem=function(elem_id)
{var elem=document.getElementById(elem_id);if(!elem){return(-1);}
if(elem.style.display=="none"){elem.style.display="";return(1);}
else{elem.style.display="none";return(0);}}
MaxFire_Dyna_Manager.prototype.show_elem=function(elem_id)
{var elem=document.getElementById(elem_id);if(elem){elem.style.display="";}}
MaxFire_Dyna_Manager.prototype.hide_elem=function(elem_id)
{var elem=document.getElementById(elem_id);if(elem){elem.style.display="none";}}
MaxFire_Dyna_Manager.prototype.get_style=function(elem,css_rule)
{var value="";if(document.defaultView&&document.defaultView.getComputedStyle){var comp_style=document.defaultView.getComputedStyle(elem,"");value=comp_style.getPropertyValue(css_rule);}
else if(elem.currentStyle){css_rule=css_rule.replace(/\-(\w)/g,function(match,p1){return p1.toUpperCase();});value=elem.currentStyle[css_rule];}
return(value);}
MaxFire_Dyna_Manager.prototype.numpix=function(value)
{return(parseInt(value)||0);}
MaxFire_Dyna_Manager.prototype.page_width=function()
{var size=this.compute_window_size();return(size.width);}
MaxFire_Dyna_Manager.prototype.compute_window_size=function()
{var size=new Object();size.width=window.innerWidth;size.height=window.innerHeight;var inner_space=size.width>0&&size.height>0;if(!inner_space){size.width=document.body.offsetWidth;size.height=document.body.offsetHeight;}
return(size);}
MaxFire_Dyna_Manager.prototype.find_pos=function(elem)
{var curr_left=0;var curr_top=0;if(elem.offsetParent){do{curr_left+=elem.offsetLeft;curr_top+=elem.offsetTop;}while(elem=elem.offsetParent);}
return([curr_left,curr_top]);}
MaxFire_Dyna_Manager.prototype.now_millis=function()
{return((new Date()).getTime());}


function MaxFire_Dyna_SimpleAvar(args)
{this.run_mode="time";this.run_time=250;this.run_tick=25;this.cb_func="";this.start_val=0;this.stop_val=0;this.dist_val=0;this.dist_mag=0;this.min_zero=false;this.positive=true;this.step_count=0;this.step_def=1;this.step_val=0;this.step_mag=0;this.stair_val=0;this.dom_elem=0;this.dom_field='';this.dom_suffix='px';this.dom_refresh=false;this.dom_auto=true;if(args){for(key in args){this[key]=args[key];}}
return(this);}
MaxFire_Dyna_SimpleAvar.prototype.clear=function()
{}
MaxFire_Dyna_SimpleAvar.prototype.run=function(start_val,stop_val,step_mag,now)
{this.prepare(start_val,stop_val,step_mag,now);this.begin();}
MaxFire_Dyna_SimpleAvar.prototype.prepare=function(start_val,stop_val,step_mag,now)
{if(typeof(step_mag)=='undefined'){step_mag=0;}
if(typeof(now)=='undefined'){now=0;}
this.run_mode=step_mag<=0?'time':'step';if(start_val=='curr'){start_val=this.dom_curr_intval();}
if(stop_val=='curr'){stop_val=this.dom_curr_intval();}
this.start_val=start_val;this.stop_val=stop_val;this.dist_val=stop_val-start_val;this.dist_mag=Math.abs(this.dist_val);this.positive=this.dist_val>0;if(step_mag==0){step_mag=this.step_def;}
step_mag=Math.abs(step_mag);this.step_mag=step_mag;this.step_val=this.positive?step_mag:-step_mag;this.step_count=0;this.stair_val=0;this.debug_buf=[];this.has_ended=false;this.curr_val=this.start_val;}
MaxFire_Dyna_SimpleAvar.prototype.begin=function(now)
{if(typeof(now)=='undefined'){now=0;}
this.start_time=now||dyna.now_millis();this.timer=setInterval(this.cb_func,this.run_tick);this.debug_buf=[];this.dom_autoshow();}
MaxFire_Dyna_SimpleAvar.prototype.chug=function()
{this.timer=setTimeout(this.cb_func,this.run_tick);}
MaxFire_Dyna_SimpleAvar.prototype.monitor=function(now)
{this.now=now||dyna.now_millis();this.elapsed_time=this.now-this.start_time;var pct_done=0;if(this.run_mode=='step'){this.stair_val+=this.step_val;pct_done=this.stair_val/this.dist_val;}
else{pct_done=this.elapsed_time/this.run_time;}
this.move_pct(pct_done);return(pct_done);}
MaxFire_Dyna_SimpleAvar.prototype.move_pct=function(pct_done)
{this.pct_done=pct_done;var incr_val=parseInt(pct_done*this.dist_val);var new_val=this.start_val+incr_val;return(this.move_to(new_val));}
MaxFire_Dyna_SimpleAvar.prototype.move_sync=function(avar)
{var new_val=this.curr_val+avar.delta_val;return(this.move_to(new_val));}
MaxFire_Dyna_SimpleAvar.prototype.move_delta=function(delta_val)
{var new_val=this.curr_val+delta_val;return(this.move_to(new_val));}
MaxFire_Dyna_SimpleAvar.prototype.move_to=function(curr_val)
{++this.step_count;if(this.positive){this.done=curr_val>=this.stop_val;}
else{this.done=curr_val<=this.stop_val;}
this.not_done=!this.done;if(this.done){curr_val=this.stop_val;}
if(this.min_zero&&curr_val<0){curr_val=0;}
this.prev_val=this.curr_val;this.delta_val=curr_val-this.prev_val;this.curr_val=curr_val;this.debug_buf.push(this.step_count+" | "+curr_val+" | "+this.delta_val+" | "+this.pct_done);return(curr_val);}
MaxFire_Dyna_SimpleAvar.prototype.update=function()
{this.refresh();}
MaxFire_Dyna_SimpleAvar.prototype.end=function()
{clearInterval(this.timer);this.stop_time=dyna.now_millis();this.curr_val=this.stop_val;this.refresh();this.timer=0;this.has_ended=true;this.dom_autohide();}
MaxFire_Dyna_SimpleAvar.prototype.refresh=function()
{if(this.dom_refresh){this.dom_elem.style[this.dom_field]=this.curr_val+this.dom_suffix;}}
MaxFire_Dyna_SimpleAvar.prototype.debug=function()
{alert(this.debug_buf.join("\n"));}
MaxFire_Dyna_SimpleAvar.prototype.dom_autoshow=function()
{var active=this.dom_refresh&&this.dom_auto;if(!active){return(false);}
this.dom_elem.style.display="";}
MaxFire_Dyna_SimpleAvar.prototype.dom_autohide=function()
{var active=this.dom_refresh&&this.dom_auto;if(!active){return(false);}
if(this.dom_collapsed()){this.dom_elem.style.display="none";}}
MaxFire_Dyna_SimpleAvar.prototype.dom_collapsed=function()
{return(parseInt(this.dom_elem.style.width)==0||parseInt(this.dom_elem.style.height)==0);}
MaxFire_Dyna_SimpleAvar.prototype.dom_curr_value=function()
{return(this.dom_elem.style[this.dom_field]);}
MaxFire_Dyna_SimpleAvar.prototype.dom_curr_intval=function()
{return(parseInt(this.dom_elem.style[this.dom_field]));}


function MaxFire_Host_Manager(args)
{this.name=window.location.hostname;if(args){for(key in args){this[key]=args[key];}}
return(this);}
MaxFire_Host_Manager.prototype.prep=function()
{return(this);}
MaxFire_Host_Manager.prototype.is_www=function()
{return(this.name=="www.maxim-ic.com"||this.name=="www.maximintegrated.com");}
MaxFire_Host_Manager.prototype.is_support=function()
{return(this.name=="support.maxim-ic.com"||this.name=="support.maximintegrated.com");}
MaxFire_Host_Manager.prototype.is_para=function()
{return(this.name=="para.maxim-ic.com"||this.name=="para.maximintegrated.com");}
MaxFire_Host_Manager.prototype.is_mymaxim=function()
{return(this.name=="my.maxim-ic.com"||this.name=="my.maximintegrated.com");}
MaxFire_Host_Manager.prototype.is_ssl=function()
{var url=window.location.href;var flag=url.match(/https\:/i)?1:0;return(flag);}


function MaxFire_Lang_Manager(args)
{this.main_reg={'en':{},'cn':{},'jp':{},'kr':{},'ru':{}};this.code='en';this.curr=this.main_reg[this.code];if(args){for(key in args){this[key]=args[key];}}
return(this);}
MaxFire_Lang_Manager.prototype.prep=function()
{return(this);}
MaxFire_Lang_Manager.prototype.select=function(code)
{if(!this.main_reg[code]){return(false);}
this.curr=this.main_reg[code];this.code=code;return(this.curr);}
MaxFire_Lang_Manager.prototype.get_code=function(){return(this.code);}
MaxFire_Lang_Manager.prototype.poll_code=function()
{return(this.poll_lang_switch_code());}
MaxFire_Lang_Manager.prototype.poll_lang_switch_code=function()
{var form=document.scp_form;if(!form){return('en');}
var menu=form.lsw_menu;if(!menu){return('en');}
var option=menu.options[menu.selectedIndex];var tmp=option.innerHTML.match(/\((\w+)\)/);var lang=RegExp.$1||'en';return(lang);}


var snp_active=1;var snp_motion=1;var snp_drawer=0;var snp_router=0;function MaxFire_Navpad_Manager(args)
{this.drawer=0;this.router=0;this.good2go=0;this.mid=-1;this.lang=Lang.get_code();this.sname='';this.section='';this.secmap={};this.router_elem=document.getElementById('snp-router');if(typeof(MaxFire_Drawer_Manager)=="function"){this.drawer=new MaxFire_Drawer_Manager({vname:"snp_drawer",name:"snp_drawer",eid:"snp-drawer",width:960,orig_height:230,height:350});}
if(typeof(MaxFire_Slideshow_Manager)=="function"){this.router=new MaxFire_Slideshow_Manager({vname:"snp_router",name:"snp-router",eid:"snp-router",width:"100%",orig_height:"200",height:"220",height:"240",height:"350",trans_style:"wisk10",slide_order:["s1","s2","s3","s4","s5","s6","s7"],curr_sname:"s1"});}
snp_drawer=this.drawer;snp_router=this.router;if(args){for(key in args){this[key]=args[key];}}
return(this);}
MaxFire_Navpad_Manager.prototype.load_base=function(cb_func_str)
{var proto=Host.is_ssl()?'https':'http';var host="www.maximintegrated.com";if(Host&&Host.name.match(/\.maxim-ic\.com/)){host="www.maxim-ic.com";}
Ajax.load_xcontent(cb_func_str,proto+"://"+host+"/navman/wsi/navpad/base.mvp",{mid:this.mid,lang:this.lang,sname:this.sname},"");}
MaxFire_Navpad_Manager.prototype.load_section=function(cb_func_str,file_name,params)
{if(!params){params={};}
params.mid=this.mid;params.lang=this.lang;params.sname=this.sname;Ajax.load_xcontent(cb_func_str,"http://www.maxim-ic.com/navman/wsi/navpad/"+file_name,params,"cache");}
MaxFire_Navpad_Manager.prototype.prep_slides=function()
{this.router.prep_slides();this.router.display_slide(this.router.first_slide_sname(),'show');this.good2go=1;}
MaxFire_Navpad_Manager.prototype.click_tab=function(tid)
{if(this.drawer_active()){this.select_tab(tid);}
else{this.goto_tab(tid);}}
MaxFire_Navpad_Manager.prototype.select_tab=function(tid)
{this.sname=tid;var curr_sname=this.router.curr_sname;var next_sname=tid;var is_current=next_sname==curr_sname;var curr_tlink=dyna.elem('snb-'+curr_sname+'-link');var next_tlink=dyna.elem('snb-'+next_sname+'-link');var curr_tcell=dyna.elem('snb-'+curr_sname+'-cell');var next_tcell=dyna.elem('snb-'+next_sname+'-cell');var tab_on_color='#6E2580';var tab_off_color='#6C6F70';var tab_on_bgcolor='#FFFFFF';var tab_off_bgcolor='#FFFFFF';var tid_elem=dyna.elem('snb-'+tid+'-link');var edge_elem=dyna.elem('site-header-edge2');if(this.drawer.state=='anim'){return(1);}
if(this.drawer.state=='in'){if(edge_elem){edge_elem.style.display='none';}
this.router.goto_slide(next_sname);this.drawer.slide_out(tid);if(next_tlink){next_tlink.className='snb2';}
return(2);}
if(next_sname==this.router.curr_sname){this.drawer.slide_in(tid);if(next_tlink){next_tlink.className='snb1';}
if(edge_elem){edge_elem.style.display='';}
return(3);}
if(next_sname!=this.router.curr_sname){this.router.select_slide(next_sname,'wisk10');if(curr_tlink){curr_tlink.className='snb1';}
if(next_tlink){next_tlink.className='snb2';}
return(4);}
return(0);}
MaxFire_Navpad_Manager.prototype.goto_tab=function(tid)
{this.sname=tid;var eid='snb-'+tid+'-link';var elem=document.getElementById(eid);if(!elem){alert("No tab element.");return(0);}
var url=elem.href;if(!url){alert("No tab url.");return(0);}
window.location=url;}
MaxFire_Navpad_Manager.prototype.drawer_close=function(tid)
{this.select_tab(this.router.curr_sname);}
MaxFire_Navpad_Manager.prototype.slide_elem=function(sref)
{return(dyna.elem(this.router.name+'-'+sref));}
MaxFire_Navpad_Manager.prototype.panel_elem=function(pref)
{return(dyna.elem(this.router.name+'-'+this.sname+pref));}
MaxFire_Navpad_Manager.prototype.drawer_active=function()
{return(snp_active&&this.drawer_loaded()&&this.drawer_exists());}
MaxFire_Navpad_Manager.prototype.drawer_loaded=function()
{return(typeof(MaxFire_Dyna_Manager)=="function"&&typeof(MaxFire_Drawer_Manager)=="function"&&typeof(MaxFire_Slideshow_Manager)=="function");}
MaxFire_Navpad_Manager.prototype.drawer_exists=function()
{return(typeof(dyna)=="object"&&typeof(this.drawer)=="object"&&typeof(this.router)=="object");}


function MaxFire_Slideshow_Manager(args)
{if(!args.eid){return(false);}
var elem=dyna.elem(args.eid);if(!elem){return(false);}
elem.max_obj=this;++dyna.icount;if(!args.slide_order){return(false);}
var first_slide=args.slide_order[0];if(!first_slide){return(false);}
this.eid=args.eid;this.name=args.eid;this.elem=elem;this.state='default';this.width=400;this.height=200;this.curr_sname='';this.curr_pos=0;this.shutdown=false;this.trans_style='wisk10';this.scene={state:"done"};this.auto={active:false,state:'play',command:'',timer:0,intro_period:15000,view_period:10000,max_loops:1,num_loops:0,inc_loops:1};this.afk={active:true,state:'run',timer:0,wait_period:3*60000};for(key in args){this[key]=args[key];}
if(!this.vname){this.vname=this.name;}
this.cb_func=this.vname+".tick()";this.init_slide_map();this.auto_init();return(this);}
MaxFire_Slideshow_Manager.prototype.init_slide_map=function()
{var min_pos=0;var max_pos=this.slide_order.length-1;this.slide_map={};for(var i=0;i<this.slide_order.length;++i){var sname=this.slide_order[i];var rec={"sname":sname,"pos":i};rec.next_pos=i==max_pos?0:i+1;rec.prev_pos=i==min_pos?max_pos:i-1;rec.next_sname=this.slide_order[rec.next_pos];rec.prev_sname=this.slide_order[rec.prev_pos];this.slide_map[sname]=rec;}
return(this.slide_map);}
MaxFire_Slideshow_Manager.prototype.prep_slides=function()
{var elem=0;for(var i=0;i<this.slide_order.length;i++){var sname=this.slide_order[i];var sconf=this[sname];elem=dyna.elem(this.name+'-'+sname+'p1');if(elem){this.prep_panel(elem);}
elem=dyna.elem(this.name+'-'+sname+'p2');if(elem){this.prep_panel(elem);}}}
MaxFire_Slideshow_Manager.prototype.prep_panel=function(p)
{p.style.display='none';p.style.position='absolute';p.style.left='0px';p.style.top='0px';p.style.width='100%';p.style.height=this.height+'px';}
MaxFire_Slideshow_Manager.prototype.var_check=function()
{var tmp=eval("typeof("+this.vname+")");if(tmp=="undefined"){this.shutdown=true;return(false);}
return(true);}
MaxFire_Slideshow_Manager.prototype.auto_init=function()
{var big_num=1000000000;if(this.auto.max_loops<0||this.auto.max_loops=='nolimit'){this.auto.max_loops=big_num;}
this.auto.begin_func=this.vname+".auto_begin";this.auto.view_func=this.vname+".auto_view";this.auto.select_func=this.vname+".auto_select";}
MaxFire_Slideshow_Manager.prototype.auto_interrupt=function()
{this.auto_reset();this.auto_timer_clear();this.auto_inc_loops();this.auto_snooze();}
MaxFire_Slideshow_Manager.prototype.auto_intro=function()
{var cb_func=this.auto.begin_func+"()";this.afk.timer=setTimeout(cb_func,this.auto.intro_period);}
MaxFire_Slideshow_Manager.prototype.auto_snooze=function()
{var cb_func=this.auto.begin_func+"()";this.afk.timer=setTimeout(cb_func,this.afk.wait_period);}
MaxFire_Slideshow_Manager.prototype.auto_begin=function()
{this.auto_timer_clear();var first_sname=this.first_slide_sname();var view_period=this.auto.view_period;if(this.curr_sname==first_sname){view_period=1000;}
else{this.auto_select(first_sname);}
var cb_func=this.auto.view_func+"()";this.auto.timer=setTimeout(cb_func,view_period);}
MaxFire_Slideshow_Manager.prototype.auto_reset=function()
{this.auto.state="play";this.auto.num_loops=0;}
MaxFire_Slideshow_Manager.prototype.auto_timer_clear=function()
{if(this.auto.timer){clearTimeout(this.auto.timer);}
this.auto.timer=0;if(this.afk.timer){clearTimeout(this.afk.timer);}
this.afk.timer=0;}
MaxFire_Slideshow_Manager.prototype.auto_inc_loops=function()
{if(this.auto.inc_loops){this.auto.max_loops+=this.auto.inc_loops;this.auto.inc_loops=0;}}
MaxFire_Slideshow_Manager.prototype.auto_view=function()
{var command=this.auto_next();var view_period=this.auto.view_period;var first_sname=this.first_slide_sname();var next_sname=this.next_slide_sname();var view_func=this.auto.view_func+"()";if(command=="done"){this.auto_inc_loops();this.auto_select(first_sname);this.auto_interrupt();}
else if(command=="next"){this.auto_select(next_sname);this.auto.timer=setTimeout(view_func,view_period);}
else if(command=="first"){this.auto_select(first_sname);this.auto.timer=setTimeout(view_func,view_period);}
else if(command=="pause"){}
else{;}}
MaxFire_Slideshow_Manager.prototype.auto_next=function()
{this.auto_timer_clear();this.auto.command="done";if(this.auto.state=="pause"){this.auto.command="pause";return(this.auto.command);}
if(this.slide_order.length<=1){return(this.auto.command);}
if(!this.auto.active){return(this.auto.command);}
var first_sname=this.slide_order[0];var curr_srec=this.slide_map[this.curr_sname];var next_srec=this.slide_map[curr_srec.next_sname];var loop_ending=next_srec.pos==0;var cycle_ending=false;if(loop_ending){++this.auto.num_loops;cycle_ending=this.auto.num_loops>=this.auto.max_loops;if(cycle_ending){this.auto.num_loops=0;}}
if(cycle_ending){return(this.auto.command);}
this.auto.command="next";return(this.auto.command);}
MaxFire_Slideshow_Manager.prototype.auto_select=function(sname)
{this.control_update(sname);this.select_slide(sname);}
MaxFire_Slideshow_Manager.prototype.first_slide_sname=function()
{return(this.slide_order[0]);}
MaxFire_Slideshow_Manager.prototype.next_slide_sname=function()
{return(this.slide_map[this.curr_sname].next_sname);}
MaxFire_Slideshow_Manager.prototype.prev_slide_sname=function()
{return(this.slide_map[this.curr_sname].prev_sname);}
MaxFire_Slideshow_Manager.prototype.get_slide_pos=function(sname)
{for(var i=0;i<this.slide_order.length;++i){if(this.slide_order[i]==sname){return(i);}}
return(-1);}
MaxFire_Slideshow_Manager.prototype.select_next_slide=function(trans_style)
{this.select_slide(this.next_slide_sname(),trans_style);}
MaxFire_Slideshow_Manager.prototype.select_prev_slide=function(trans_style)
{this.select_slide(this.prev_slide_sname(),trans_style);}
MaxFire_Slideshow_Manager.prototype.select_slide=function(sname,trans_style)
{if(this.scene&&this.scene.state=='active'){return(false);}
if(sname==this.curr_sname){return(false);}
if(typeof(trans_style)=='undefined'){trans_style=this.trans_style;}
var valid_style=typeof(this['run_'+trans_style])!='undefined';if(!valid_style){trans_style='wisk10';}
this.init_scene(this.curr_sname,sname,trans_style);this['run_'+trans_style]();return(true);}
MaxFire_Slideshow_Manager.prototype.goto_slide=function(sname)
{if(sname==this.curr_sname){return(false);}
var elem=0;elem=dyna.elem(this.name+'-'+sname+'p1');if(elem){elem.style.left="0px";}
elem=dyna.elem(this.name+'-'+sname+'p2');if(elem){elem.style.left="0px";}
this.display_slide(this.curr_sname,'hide');this.curr_sname=sname;this.display_slide(this.curr_sname,'show');return(true);}
MaxFire_Slideshow_Manager.prototype.click_slide=function(sname)
{var eid=this.name+"-"+sname+"p1";var elem=dyna.elem(eid+"-link");if(!elem){alert("ERROR: No elem found: "+eid);return(false);}
var slide_url=elem.href;if(!slide_url){alert("ERROR: No url found: "+eid);return(false);}
this.auto_timer_clear();window.location=slide_url;}
MaxFire_Slideshow_Manager.prototype.control_next_slide=function()
{this.control_select_slide(this.next_slide_sname());}
MaxFire_Slideshow_Manager.prototype.control_prev_slide=function()
{this.control_select_slide(this.prev_slide_sname());}
MaxFire_Slideshow_Manager.prototype.control_select_slide=function(sname)
{this.auto_interrupt();this.control_update(sname);this.select_slide(sname);}
MaxFire_Slideshow_Manager.prototype.control_update=function(next_sname)
{var curr_sname=this.curr_sname;var curr_btn=dyna.elem(this.name+"-"+curr_sname+"-btn");var next_btn=dyna.elem(this.name+"-"+next_sname+"-btn");if(curr_btn){curr_btn.innerHTML="&#9633;";}
if(next_btn){next_btn.innerHTML="&#9632;";}}
MaxFire_Slideshow_Manager.prototype.tick=function()
{var rv=this['run_'+this.scene.style]();return(rv);}
MaxFire_Slideshow_Manager.prototype.run_wisk10=function()
{var scene=this.scene;if(this.shutdown||!scene){return(false);}
var a1=scene.a1;var a2=scene.a2;var b1=scene.b1;var b2=scene.b2;if(scene.stage==0.0){var slide_width=a1.offsetWidth;b1.style.left=slide_width+"px";b2.style.left=slide_width+"px";b1.style.display="";b2.style.display="";a1.max_avar=this.init_avar(500,5,a1,'left');a2.max_avar=this.init_avar(500,5,a2,'left');b1.max_avar=this.init_avar(500,5,b1,'left');b2.max_avar=this.init_avar(500,5,b2,'left');a1.max_avar.prepare('curr',-slide_width);a2.max_avar.prepare('curr',-slide_width);b1.max_avar.prepare('curr',0);b2.max_avar.prepare('curr',0);scene.stage=1.0;a1.max_avar.begin();}
else if(scene.stage==1.0){a1.max_avar.monitor();if(a1.max_avar.not_done){a1.max_avar.update();a2.max_avar.move_delta(a1.max_avar.delta_val);a2.max_avar.update();b1.max_avar.move_delta(a1.max_avar.delta_val);b1.max_avar.update();b2.max_avar.move_delta(a1.max_avar.delta_val);b2.max_avar.update();}
else{a1.max_avar.end();a1.style.display="none";a2.style.display="none";b1.style.left="0px";b2.style.left="0px";scene.stage=2.0;scene.state='done';this.curr_sname=scene.bname;}}}
MaxFire_Slideshow_Manager.prototype.run_wisk11=function()
{var scene=this.scene;if(this.shutdown||!scene){return(false);}
var a1=scene.a1;var a2=scene.a2;var b1=scene.b1;var b2=scene.b2;if(scene.stage==0.0){var slide_width=a1.offsetWidth;var half_width=parseInt(slide_width/2);b1.style.left=slide_width+"px";b2.style.left=slide_width+"px";b1.style.display="";b2.style.display="";a1.max_avar=this.init_avar(500,5,a1,'left');a2.max_avar=this.init_avar(300,5,a2,'left');b1.max_avar=this.init_avar(500,5,b1,'left');b2.max_avar=this.init_avar(500,5,b2,'left');a1.max_avar.prepare('curr',-slide_width);a2.max_avar.prepare('curr',-half_width);b1.max_avar.prepare('curr',0);b2.max_avar.prepare('curr',0);scene.stage=1.0;a2.max_avar.begin();}
else if(scene.stage==1.0){a2.max_avar.monitor();if(a2.max_avar.not_done){a2.max_avar.update();}
else{a2.max_avar.end();a2.style.display="none";scene.stage=2.0;a1.max_avar.begin();}}
else if(scene.stage==2.0){a1.max_avar.monitor();if(a1.max_avar.not_done){a1.max_avar.update();b1.max_avar.move_delta(a1.max_avar.delta_val);b1.max_avar.update();b2.max_avar.move_delta(a1.max_avar.delta_val);b2.max_avar.update();}
else{var slide_width=dyna.page_width();a1.max_avar.end();a1.style.display="none";a1.style.left=slide_width;a2.style.display="none";a2.style.left=slide_width+"px";b1.style.left="0px";b2.style.left="0px";scene.stage=3.0;scene.state='done';this.curr_sname=scene.bname;}}}
MaxFire_Slideshow_Manager.prototype.run_wisk12=function()
{var scene=this.scene;if(this.shutdown||!scene){return(false);}
var a1=scene.a1;var a2=scene.a2;var b1=scene.b1;var b2=scene.b2;if(scene.stage==0.0){var slide_width=a1.offsetWidth;var half_width=parseInt(slide_width/2);b1.style.left=slide_width+"px";b2.style.left=slide_width+"px";b1.style.display="";b2.style.display="";a1.max_avar=this.init_avar(1000,1,a1,'left');a2.max_avar=this.init_avar(1800,1,a2,'left');b1.max_avar=this.init_avar(1000,1,b1,'left');b2.max_avar=this.init_avar(1000,1,b2,'left');a1.max_avar.prepare('curr',-slide_width);a2.max_avar.prepare('curr',-half_width);b1.max_avar.prepare('curr',0);b2.max_avar.prepare('curr',0);scene.stage=1.0;a2.max_avar.begin();}
else if(scene.stage==1.0){a2.max_avar.monitor();if(a2.max_avar.not_done){a2.max_avar.update();}
if(a2.max_avar.pct_done>0.40){scene.stage=2.0;a1.max_avar.begin();}}
else if(scene.stage==2.0){a2.max_avar.monitor();if(a2.max_avar.not_done){a2.max_avar.update();}
else{var slide_width=dyna.page_width();a2.max_avar.end();a2.style.display="none";a2.style.left=slide_width+"px";}
a1.max_avar.monitor();if(a1.max_avar.not_done){a1.max_avar.update();b1.max_avar.move_delta(a1.max_avar.delta_val);b1.max_avar.update();b2.max_avar.move_delta(a1.max_avar.delta_val);b2.max_avar.update();}
else{var slide_width=dyna.page_width();a1.max_avar.end();a1.style.display="none";a1.style.left=slide_width+"px";b1.style.left="0px";b2.style.left="0px";}
var the_end=a1.max_avar.has_ended&&a2.max_avar.has_ended;if(the_end){scene.stage=3.0;scene.state='done';this.curr_sname=scene.bname;}}}
MaxFire_Slideshow_Manager.prototype.run_innout10=function()
{var scene=this.scene;if(this.shutdown||!scene){return(false);}
var a1=scene.a1;var a2=scene.a2;var b1=scene.b1;var b2=scene.b2;if(scene.stage==0.0){var slide_width=a1.offsetWidth;b1.style.left=slide_width+"px";b2.style.left=-slide_width+"px";b1.style.display="";b2.style.display="";a1.max_avar=this.init_avar(500,5,a1,'left');a2.max_avar=this.init_avar(600,5,a2,'left');b1.max_avar=this.init_avar(500,5,b1,'left');b2.max_avar=this.init_avar(300,5,b2,'left');a1.max_avar.prepare('curr',-slide_width);a2.max_avar.prepare('curr',-slide_width);b1.max_avar.prepare('curr',0);b2.max_avar.prepare('curr',0);scene.stage=1.0;a2.max_avar.begin();}
else if(scene.stage==1.0){a2.max_avar.monitor();if(a2.max_avar.not_done){a2.max_avar.update();}
else{a2.max_avar.end();a2.style.display="none";scene.stage=2.0;a1.max_avar.begin();}}
else if(scene.stage==2.0){a1.max_avar.monitor();if(a1.max_avar.not_done){a1.max_avar.update();b1.max_avar.move_delta(a1.max_avar.delta_val);b1.max_avar.update();}
else{var slide_width=dyna.page_width();a1.max_avar.end();a1.style.display="none";a1.style.left=slide_width;b1.style.left="0px";b1.style.top="0px";scene.stage=3.0;b2.max_avar.begin();}}
else if(scene.stage==3.0){b2.max_avar.monitor();if(b2.max_avar.not_done){b2.max_avar.update();}
else{b2.max_avar.end();b2.style.left="0px";b2.style.top="0px";scene.stage=4.0;scene.state='done';this.curr_sname=scene.bname;}}}
MaxFire_Slideshow_Manager.prototype.init_scene=function(aname,bname,style)
{var good_var=this.var_check();if(!good_var){return(false);}
var scene={};this.scene=scene;scene.state='active';scene.style=style;scene.stage=0.0;scene.aname=aname;scene.bname=bname;var aref=this.name+'-'+aname;scene.a1=dyna.elem(aref+'p1');scene.a2=dyna.elem(aref+'p2');var bref=this.name+'-'+bname;scene.b1=dyna.elem(bref+'p1');scene.b2=dyna.elem(bref+'p2');return(scene);}
MaxFire_Slideshow_Manager.prototype.init_avar=function(run_time,run_tick,dom_elem,dom_field)
{return(new MaxFire_Dyna_SimpleAvar({run_time:run_time,run_tick:run_tick,cb_func:this.cb_func,step_def:20,dom_refresh:true,dom_auto:true,dom_elem:dom_elem,dom_field:dom_field}));}
MaxFire_Slideshow_Manager.prototype.display_slide=function(sname,mode)
{var val=mode=='show'?'':'none';var elem=0;elem=dyna.elem(this.name+'-'+sname+'p1');if(elem){elem.style.display=val;}
elem=dyna.elem(this.name+'-'+sname+'p2');if(elem){elem.style.display=val;}}


function MaxFire_Url_Manager(args)
{this.curr_url=window.location.href;if(args){for(key in args){this[key]=args[key];}}
return(this);}
MaxFire_Url_Manager.prototype.prep=function()
{}
MaxFire_Url_Manager.prototype.shift_mymaxim_wsi=function(uri)
{return(Host.is_mymaxim()?uri:"/mymaxim"+uri);}


var Univ=new Object();Univ.version='2.1';var App=0;var Mary=0;var Agent=0;var Ajax=0;var Dyna=0;var dyna=0;var Host=0;var Lang=0;var Url=0;function init_major_vars()
{if(typeof(MaxFire_Dyna_Manager)=="function"){dyna=new MaxFire_Dyna_Manager();}
if(typeof(MaxFire_Agent_Manager)=="function"){Agent=new MaxFire_Agent_Manager();}
if(typeof(MaxFire_Ajax_Manager)=="function"){Ajax=new MaxFire_Ajax_Manager();}
if(typeof(MaxFire_Host_Manager)=="function"){Host=new MaxFire_Host_Manager();}
if(typeof(MaxFire_Lang_Manager)=="function"){Lang=new MaxFire_Lang_Manager();}
if(typeof(MaxFire_Url_Manager)=="function"){Url=new MaxFire_Url_Manager();}}
function prep_major_vars()
{if(Lang){Lang.prep();}}
function add_load_func(func)
{var curr_onload=window.onload;if(typeof(curr_onload)!='function'){window.onload=func;}
else{window.onload=function(){curr_onload();func();};}}
function toggle_display(elem_id){return(dyna.toggle_elem(elem_id));}
function toggle_elem(elem_id){return(dyna.toggle_elem(elem_id));}
function show_elem(elem_id){return(dyna.show_elem(elem_id));}
function hide_elem(elem_id){return(dyna.hide_elem(elem_id));}
function safe_submit_form(form,btn_name)
{if(!btn_name){btn_name="submit_btn";}
var elem=form[btn_name];if(!elem){return(true);}
elem.disabled=true;elem.value="Please wait...";return(true);}
function track_pdf_click(link_elem)
{if(_elq&&link_elem){_elq.trackEvent(link_elem.href);}
return(false);}
function debug_pdf_click(link_elem)
{alert("debug_pdf_click:\n\n"
+"--- _elq ----------------------------\n"+_elq+"\n\n"
+"--- link_elem -----------------------\n"+typeof(link_elem)+"\n\n"
+"--- link_elem.href ------------------\n"+link_elem.href+"\n\n");return(false);}
function is_mymaxim_host(){return(Host.is_mymaxim());}
function host_is_mymaxim(){return(Host.is_mymaxim());}
function shift_mymaxim_client_uri(uri){return(Url.shift_mymaxim_wsi(uri));}
function update_infopref(mid,info_type,info_id,action,src_part)
{if(mid==undefined){mid=Mary[1];}
if(!Mary[0]||mid<=0){return("");}
var uri=shift_mymaxim_client_uri("/wsi/infoprefs/pref-update.mvp");Ajax.init_get({obj:0,action_id:"update_infopref",handler:update_infopref_handler,url:uri,params:{mid:mid,info_type:info_type,info_id:info_id,action:action,src_part:src_part}});Ajax.run(1);return(1);}
function update_infopref_handler()
{var this_obj=Ajax.obj||Ajax;if(!Ajax.is_done()){return("");}
var mdata=Ajax.process_json_response();var wrapper=Ajax.wrapper;if(!wrapper.success){return(Ajax.report_error("Update Failed."));}
var info_key=Ajax.params.info_type+" "+Ajax.params.info_id;var blip_msg="Preference Updated";Ajax.blip_feedback("update_infopref_blip_"+Ajax.params.info_id,blip_msg);return(1);}
function update_subscription(mid,channel_id,action)
{if(mid==undefined){mid=Mary[1];}
if(!Mary[0]||mid<=0){return("");}
var uri=shift_mymaxim_client_uri("/wsi/sub/sub-update.mvp");Ajax.init_get({obj:App,action_id:"update_subscription",handler:update_subscription_handler,url:uri,params:{mid:mid,channel_id:channel_id,action:action}});Ajax.run(1);return(1);}
function update_subscription_handler()
{var this_obj=Ajax.obj||Ajax;if(!Ajax.is_done()){return("");}
var mdata=Ajax.process_json_response();var wrapper=Ajax.wrapper;if(!wrapper.success){return(Ajax.report_error("Update Failed."));}
Ajax.blip_feedback("update_subscription_blip","Subscription Updated");return(1);}
function add_bookmark(mid,folder)
{if(!folder){folder="Inbox";}
if(mid==undefined){mid=Mary[1];}
if(!Mary[0]||mid<=0){return("");}
var uri=shift_mymaxim_client_uri("/wsi/bookmarks/add.mvp");Ajax.init_get({obj:0,action_id:"add_bookmark",handler:add_bookmark_handler,url:uri,params:{mid:mid,folder:folder,url:document.URL,title:document.title}});Ajax.run(1);return(1);}
function add_bookmark_handler()
{var this_obj=Ajax.obj||Ajax;if(!Ajax.is_done()){return("");}
var mdata=Ajax.process_json_response();var wrapper=Ajax.wrapper;if(!wrapper.success){return(Ajax.report_error("Update Failed."));}
var ack_msg="Bookmark Added";var elem=document.getElementById("add_bookmark_ack");if(elem){elem.innerHTML=ack_msg;}
else{alert(ack_msg);}
return(1);}

