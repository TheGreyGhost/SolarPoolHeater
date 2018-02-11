$(document).ready(
    function() {
	$("#product-tree-button").click(function () {
		$("#product-tree-container").animate({width: 'toggle'});;
	});
	$("#product-tree-close").click(function () {
		$("#product-tree-container").animate({width: 'toggle'});;
	});
	$("#new-products-button").click(function () {
		$("#new-products-container").animate({width: 'toggle'});;
	});
	$("#new-products-close").click(function () {
		$("#new-products-container").animate({width: 'toggle'});;
	});
	$("#related-products-button").click(function () {
		$("#related-products-container").animate({width: 'toggle'});;
	});
	$("#related-products-close").click(function () {
		$("#related-products-container").animate({width: 'toggle'});;
	});
	$("#related-new-products-button").click(function () {
		$("#related-new-products-container").animate({width: 'toggle'});;
	});
	$("#related-new-products-close").click(function () {
		$("#related-new-products-container").animate({width: 'toggle'});;
	});
	$("#solutions-button").click(function () {
		$("#solutions-container").animate({width: 'toggle'});;
	});
	$("#solutions-close").click(function () {
		$("#solutions-container").animate({width: 'toggle'});;
	});
	$("#products-by-application-button").click(function () {
		$("#products-by-application-container").animate({width: 'toggle'});;
	});
	$("#products-by-application-close").click(function () {
		$("#products-by-application-container").animate({width: 'toggle'});;
	});
	$("#new-app-notes-button").click(function () {
		$("#new-app-notes-container").animate({width: 'toggle'});;
	});
	$("#new-app-notes-close").click(function () {
		$("#new-app-notes-container").animate({width: 'toggle'});;
	});
	$("#technical-docs-button").click(function () {
		$("#technical-docs-container").animate({width: 'toggle'});;
	});
	$("#technical-docs-close").click(function () {
		$("#technical-docs-container").animate({width: 'toggle'});;
	});
	$("#reference-designs-button").click(function () {
		$("#reference-designs-container").animate({width: 'toggle'});;
	});
	$("#reference-designs-close").click(function () {
		$("#reference-designs-container").animate({width: 'toggle'});;
	});
	$("#training-videos-button").click(function () {
		$("#training-videos-container").animate({width: 'toggle'});;
	});
	$("#training-videos-close").click(function () {
		$("#training-videos-container").animate({width: 'toggle'});;
	});
	$("#events-button").click(function () {
		$("#events-container").animate({width: 'toggle'});;
	});
	$("#events-close").click(function () {
		$("#events-container").animate({width: 'toggle'});;
	});
	$("#design-guides-button").click(function () {
		$("#design-guides-container").animate({width: 'toggle'});;
	});
	$("#design-guides-close").click(function () {
		$("#design-guides-container").animate({width: 'toggle'});;
	});
	$("#packaging-flyout-button").click(function () {
		$("#packaging-flyout-container").animate({width: 'toggle'});;
	});
	$("#packaging-flyout-close").click(function () {
		$("#packaging-flyout-container").animate({width: 'toggle'});;
	});
	$("#tools-models-button").click(function () {
		$("#tools-models-container").animate({width: 'toggle'});;
	});
	$("#tools-models-close").click(function () {
		$("#tools-models-container").animate({width: 'toggle'});;
	});
	$("#parts-used-button").click(function () {
		$("#parts-used-container").animate({width: 'toggle'});;
	});
	$("#parts-used-close").click(function () {
		$("#parts-used-container").animate({width: 'toggle'});;
	});
});

	var prev='';
	var menu = '';
	var curr = '';
	var num = 0;
	var num2 = 0;
$(document).ready(function(){
	var wgt = $('#midarea').width();
	var wmenu = $('#divmenu').width();
	var outmenu = wgt - wmenu;
	$('#outmenu').css('width', wmenu + 'px');
	var hgt = $('#midarea').height();
	var phgt = $('#relResources').height();
	$('.col-sm-2').each(function(){
		$(this).css('height', hgt + 'px');
	});
	$('#divbnk').css('height', (hgt - phgt) + 'px');
	$('.col-sm-2 .aResource').each(function(){
		$(this).click(function(){
			//alert('6');
			curr = 'res';
			closesearch();
			closemenu();
			var qprv = prev;
			//alert(qprv);
			if(prev != '' && prev != $(this).prev().attr('id')){
				curr = '';
				prev = '';				
				$('#'+qprv).find('.rightcol-close').click();						
			}
			if($(this).prev().attr('id') != undefined &&  qprv != $(this).prev().attr('id')){
				prev = $(this).prev().attr('id');			
			}
			else{
				prev = '';
			}
			//alert(prev);
		});
	});
	$('.col-sm-2 .rightcol-close').each(function(){		
		$(this).click(function(){
			//alert('Close: ' + prev);
			prev = '';
		});
	});
	$('#midarea .ib-breadcrumb').each(function(){
		$(this).find('a').click(function(event){
			curr = 'men';
			if(menu != '' && menu != $(this).next().attr('id')){
				$('#'+menu).slideUp();
				menu = '';
			}
			if($(this).next() != undefined && $(this).next() != null){
				menu = $(this).next().attr('id');
			}
			else{
				menu = '';
			}
		});
	});
	$('.navbar').click(function(){
		//alert('1');
		num2++;
		if(num < num2 && $('#parametricsearchinfo').css('display') == 'block')
			closesearch();
		if(num <= (num2 - 1))
			num2 = num2 - 1;
		closeresource();
		closemenu();
	});
	$('#midarea').click(function(){
		//alert('2');
		closeresource();
		closesearch();
	});
	$('#leftmenu').click(function(){
		//alert('3');
		closemenu();
		closeresource();
		closesearch();
	});
	$('#footer').click(function(){
		//alert('4');
		closemenu();
		closeresource();
		closesearch();
	});
	$('#divContent').click(function(){
		//alert('5');
		closemenu();
	});
	$('#divbnk').click(function(){
		//alert('4');
		closemenu();
		closeresource();
		closesearch();
	});
	$('#parametricsearch').click(function(){
		num++;
	});
	$('#outmenu').click(function(){
		closemenu();
	});
});
function closeresource(){
	curr = 'close';
	if(prev != ''){
		$('#'+prev).find('.rightcol-close').click();
	}
	prev = '';
}
function closesearch(){
	if($('#parametricsearchinfo').css('display') == 'block'){
		//$('#parametricsearchinfo').css('display', 'none');
		num = 0;
		num2 = 0;
		$('#parametricsearchinfo').slideUp();
	}	
}
function closemenu(){
	if(menu != ''){
		$('#'+menu).slideUp();
	}
	menu = '';
}