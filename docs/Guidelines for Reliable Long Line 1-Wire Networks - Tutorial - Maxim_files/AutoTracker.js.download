//
//				    .ooooo.          ooo. .oo.     .ooooo.    oooo d8b
//				   d88' `88b         `888P"Y88b   d88' `88b   `888""8P
//				   888888888  88888   888   888   888   888    888
//				   888        88888   888   888   888   888    888       
//				   `"88888"          o888o o888o  `Y8bod8P'   d888b      
//

//***************************************************************************************************
//     Script for tracking file downloads, outbound links, and email clicks
//     Author: Brian J Clifton @ Omega Digital Media Ltd
//     Adjusted by Allaedin Ezzedin @ E-Nor to make the code work with Asynchronous & Event Tracking
//     MODIFIED BY MAXIM: to allow for Eloqua integration (pdf tracking).
//***************************************************************************************************

function addLinkerEvents(){
	var as = document.getElementsByTagName("a");
	var extDoc = [".doc", ".docx", ".xls", ".xlsx", ".xlsm", ".ppt", ".pptx", ".exe", ".zip", ".ZIP", ".pdf", ".PDF",  ".js", ".txt", ".wav", ".mp3", ".mov", ".mpg", ".avi", ".wmv", ".cir", ".CIR", ".ibs", ".IBS", ".fam", ".FAM", ".swf", ".mobi", ".msi", ".LIB"];
	
	// List of local sites that should not be treated as an outbound link. Include at least your own domain here
	if (location.hostname.indexOf("maximintegrated.com") > -1) {
		var extTrack = ["maximintegrated.com"];
		}
	else if (location.hostname.indexOf("transim.com") > -1) {
		var extTrack = ["transim.com"];
		}
	else {
		var extTrack = [];
		}

	for (var i = 0; i < as.length; i++) {
		var flag = 0;
		var tmp = as[i].getAttribute("onclick");
		
		if (tmp != null) {
			tmp = String(tmp);
			if (tmp.indexOf('_gaq.push') > -1) 
				continue;
		}
		
		// Tracking outbound links off site - not the GATC
		for (var j = 0; j < extTrack.length; j++) {
			if (as[i].href.indexOf(extTrack[j]) == -1 && as[i].href.indexOf('google-analytics.com') == -1) {
				flag++;
			}
		}
		
		if (flag == extTrack.length && as[i].href.indexOf("mailto:") == -1) {
			as[i].onclick = function(){
				var splitResult = this.href.split("//");
			    _gaq.push(['_trackEvent', 'Outbound Links', splitResult[1]]) + ';' + ((tmp != null) ? tmp + ';' : '');
				_gaq.push(['_trackPageview', '/vp/ext/' + splitResult[1]]) + ';' + ((tmp != null) ? tmp + ';' : '');
			};
		}
		
		// Tracking electronic documents - doc, xls, pdf, exe, zip
		for (var j = 0; j < extDoc.length; j++) {
			if (as[i].href.indexOf(extTrack[0]) != -1 && as[i].href.indexOf(extDoc[j]) != -1) {
				var is_pdf = extDoc[j] == '.pdf' || extDoc[j] == '.PDF';
				var can_track_pdf = is_pdf? typeof(track_pdf_click) == "function" : 0;
				var link_elem = as[i]; // note: must create a local var for Eloqua to work.
				if (is_pdf && can_track_pdf) {
				    as[i].onclick = function() {
					    var splitResult = this.href.split(extTrack[0]);
					    _gaq.push(['_trackEvent', 'Downloads', splitResult[1]]) + ';' + ((tmp != null) ? tmp + ';' : '');
					    _gaq.push(['_trackPageview', '/vp/downloads/' + splitResult[1]]) + ';' + ((tmp != null) ? tmp + ';' : '');
					    track_pdf_click(link_elem); // Eloqua Integration Point
					    //return (false);
				    }
				}
				else {
				    as[i].onclick = function() {
					    var splitResult = this.href.split(extTrack[0]);
					    _gaq.push(['_trackEvent', 'Downloads', splitResult[1]]) + ';' + ((tmp != null) ? tmp + ';' : '');
					    _gaq.push(['_trackPageview', '/vp/downloads/' + splitResult[1]]) + ';' + ((tmp != null) ? tmp + ';' : '');
					    //return (false);
				    }
				}
				break;
			}
		}
		
		// Tracking email clicks
		if (as[i].href.indexOf("mailto:") != -1) {
			as[i].onclick = function(){
				var splitResult = this.href.split(":");
				_gaq.push(['_trackEvent', 'Email Clicks', splitResult[1]]) + ';' + ((tmp != null) ? tmp + ';' : '');
				_gaq.push(['_trackPageview', '/vp/mailto/' + splitResult[1]]) + ';' + ((tmp != null) ? tmp + ';' : '');
			}
		}
	}
}

// This function sets GA custom variable 1 to the visitor state.
function set_GA_CV1() {
	var pageTracker = _gat._getTrackerByName();
	var cv1 = pageTracker._getVisitorCustomVar(1);
	if ( (cv1 == undefined) || (cv1 == "") ) {
		_gaq.push(['_setCustomVar', 1, 'User-type', 'Member', 1]);
		_gaq.push(['_trackEvent', 'Login - Member', 'Login - Member', 'Login - Member', 0, true]);
	}
	else if (cv1 != "Member") {
		_gaq.push(['_setCustomVar', 1, 'User-type', 'Member', 1]);
		_gaq.push(['_trackEvent', 'Login - Member', 'Login - Member', 'Login - Member', 0, true]);
	}
}
