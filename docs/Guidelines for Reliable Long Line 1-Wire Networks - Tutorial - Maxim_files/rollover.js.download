/*
	Standards Compliant Rollover Script
	Author : Lauren Kelly
	http://morphosinc.com/maxim/product.php
*/
$(document).ready(function(){
	$('#relResources .right-module').each(function(){
		var srcon = $(this).find('.imgover').attr('src');
		var ftypeon = srcon.substring(srcon.lastIndexOf('.'), srcon.length);
		var hsrcon = srcon.replace(ftypeon, '_o'+ftypeon);
		$(this).find('.right-module-img').append('<img class="imgoveron" src="'+hsrcon+'" style="display:none; border:none;" />');
		
		$(this).hover(function(){
			$(this).find('.imgover').attr('style', 'display:none;');
			$(this).find('.imgoveron').attr('style', 'display:block; border:none;');
		},function(){
			$(this).find('.imgover').attr('style', 'display:block; border:none;');
			$(this).find('.imgoveron').attr('style', 'display:none;');
		});
	});
});