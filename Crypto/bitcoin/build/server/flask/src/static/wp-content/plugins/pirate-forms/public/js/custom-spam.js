/* global pf */
/* global jQuery */
(function($, pf){

    $(document).ready(function() {
        onDocumentReady();
    });

    $(window).load(function() {
        onWindowLoad();
    });

    function onDocumentReady() {
        // fired when a form is changed from the inspector.
        if(pf.spam.gutenberg === 1){
            jQuery('body').delegate('.pirate-forms-maps-custom', 'addCustomSpam', function(){
                var i = 0;
                addCustomSpam(i++, jQuery(this));
            });
        }

        // for the front end.
        jQuery('.pirate-forms-maps-custom').each(function(i){
            addCustomSpam(i, jQuery(this));
        });
    }

    function onWindowLoad() {
        // fired when a saved form is loaded in gutenberg.
        if(pf.spam.gutenberg === 1){
            jQuery('.pirate-forms-maps-custom').each(function(i){
                addCustomSpam(i, jQuery(this));
            });
        }
    }

    function addCustomSpam(i, object){
        var $id = 'xobkcehc-' + i;
        object.empty().html(jQuery('<input id="' + $id + '" name="xobkcehc" type="' + 'xobkcehc'.split('').reverse().join('') + '" value="' + pf.spam.value + '"><label for="' + $id + '"><span>' + pf.spam.label + '</span></label>'));
    }
})(jQuery, pf);
