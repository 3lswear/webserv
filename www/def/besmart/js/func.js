
$(document).ready(function() {
	$('.slider').jcarousel( {
		initCallback: slider_initCallback,
		itemFirstInCallback: slider_firstInCallback,
		scroll: 1,
		auto: 5,
		wrap: 'both',
		// This tells jCarousel NOT to autobuild prev/next buttons
		buttonNextHTML: null,
		buttonPrevHTML: null
	});
});

function slider_initCallback (carousel) {
	
	$('.slider-nav a').bind('click', function() {
		carousel.scroll($.jcarousel.intval($(this).text()));
		return false;
	});
}

function slider_firstInCallback(carousel, item, idx, state) {
	$('.slider-nav a').removeClass('active');
	$('.slider-nav a').eq(idx-1).addClass('active');
}