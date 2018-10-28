/* global jQuery */
/* global pirateFormsObject */

(function($, pf){

    $( document ).ready( function () {
        onDocumentReady();
    });

    $( window ).load( function () {
        onWindowLoad();
    });

    function onDocumentReady() {
        'use strict';

        // file upload behavior.
        $( '.pirate-forms-file-upload-button' ).on( 'click', function () {
            var $button = $( this );
            $button.parent().find( 'input[type=file]' ).on( 'change', function () {
                $button.parent().find( 'input[type=text]' ).val( $( this ).val() ).change();
            } );
            $button.parent().find( 'input[type=file]' ).focus().click();
        } );

        $( '.pirate-forms-file-upload-input' ).on( 'click', function () {
            $( this ).parent().find( '.pirate-forms-file-upload-button' ).trigger( 'click' );
        } );
        $( '.pirate-forms-file-upload-input' ).on( 'focus', function () {
            $( this ).blur();
        } );

        // show errors.
        var session_var = pf.errors;
        if( (typeof session_var !== 'undefined') && (session_var !== '') && (typeof $('#contact') !== 'undefined') && (typeof $('#contact').offset() !== 'undefined') ) {
            $('html, body').animate({
                scrollTop: $('#contact').offset().top
            }, 'slow');
        }
        
        // support ajax forms.
        $('.pirate-forms-submit-button-ajax').closest('form').submit(function(){
            var form = $(this);
            var formData = new FormData(form[0]);
            ajaxStart( form );

            // remove the dynamic containers.
            $('div.pirate-forms-ajax').remove();

            $.ajax({
                url: pf.rest.submit.url,
                data: formData,
                type: 'POST',
                dataType: 'json',
                contentType: false,
                processData: false,
                beforeSend: function ( xhr ) {
                    xhr.setRequestHeader( 'X-WP-Nonce', pf.rest.nonce );
                },
                success: function(data){
                    //console.log("success");
                    //console.log(data);
                    form.find('input').val('');
                    form.find('select').val('');
                    form.find('input[type="checkbox"]').removeAttr('checked');
                    form.find('input[type="radio"]').removeAttr('checked');
                    var $time = new Date().getTime();

                    if(data.message){
                        form.closest('.pirate_forms_wrap').before('<div id="' + $time + '" class="pirate-forms-ajax pirate-forms-ajax-thankyou"></div>');
                        $('#' + $time).append(data.message);
                    }else if(data.redirect){
                        location.href = data.redirect;
                    }
                },
                error: function(data){
                    //console.log("no");
                    //console.log(data);
                    if(data.responseJSON){
                        var $time = new Date().getTime();
                        form.closest('.pirate_forms_wrap').prepend('<div id="' + $time + '" class="pirate-forms-ajax pirate-forms-ajax-errors"></div>');
                        $('#' + $time).append(data.responseJSON.error);
                    }
                },
                complete: function(){
                    ajaxStop( form );
                }
            });
            
            return false;
        });

    }
    
    function onWindowLoad() {
        'use strict';
        if ( $( '.pirate_forms_wrap' ).length ) {
            $( '.pirate_forms_wrap' ).each( function () {
                var formWidth = $( this ).innerWidth();
                var footerWidth = $( this ).find( '.pirate-forms-footer' ).innerWidth();
                if ( footerWidth > formWidth ) {
                    $( this ).find( '.contact_submit_wrap, .form_captcha_wrap, .pirateform_wrap_classes_spam_wrap' ).css( {'text-align' : 'left', 'display' : 'block' } );
                }
            } );
        }
    }

    function ajaxStart(element) {
        $(element).fadeTo( 'slow', 0.5 );
    }

    function ajaxStop(element) {
        $(element).fadeTo( 'fast', 1 );
    }

})(jQuery, pirateFormsObject);