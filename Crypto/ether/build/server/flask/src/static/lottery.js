function showLoader(loader, table)
{
    loader.show();
    table.hide();
    return true;
}

function hideLoader(loader, table)
{
    loader.hide();
    table.show();
    return false;
}

function claimETH()
{
    $("#claimETH").prop("disabled", true);
    loaderShownFlags[0] = showLoader($("#info_loader"), $("#info_table"));
    $.get( "/api/claimETH", function( data ) {
	getBalance();
    });
}

function getBalance()
{
    loaderShownFlags[0] = showLoader($("#info_loader"), $("#info_table"));
    $.get( "/api/getBalance", function( data ) {
	document.getElementById("balance").innerHTML = data;
	loaderShownFlags[0] = hideLoader($("#info_loader"), $("#info_table"));
    });
}

// ---

function deploy(level)
{
    let target = "lv" + String(level);
    let loader = $("#" + target + "_loader");
    let table = $("#" + target + "_table");
    $("#" + target + "_deploy").prop("disabled", true);
    $("#" + target).show();
    loaderShownFlags[level] = showLoader(loader, table);
    $.get( "/api/deploy/" + String(level), function( data ) {
	getData(level);
    });
}

function getData(level)
{
    let target = "lv" + String(level);
    let loader = $("#" + target + "_loader");
    let table = $("#" + target + "_table");
    if($("#" + target).is(":visible")){
	$.get( "/api/getData/" + String(level), function( data ) {
	    let json = JSON.parse(data);
	    if(json["contractAddr"]!==null){
		document.getElementById(target + "_contractAddr").innerHTML = json["contractAddr"];
		document.getElementById(target + "_flag").innerHTML = json["flag"];
		document.getElementById(target + "_played").innerHTML = json["played"];
		loaderShownFlags[level] = hideLoader(loader, table);
	    }
	});
    }
}

// ---

function testLuck(level)
{
    let target = "lv" + String(level);
    let loader = $("#" + target + "_loader");
    let table = $("#" + target + "_table");
    $("#" + target).show();
    loaderShownFlags[level] = showLoader(loader, table);
    $.get( "/api/testLuck/" + String(level), function( data ) {
	getData(level);
	getBalance();
	loaderShownFlags[level] = hideLoader(loader, table);
    });
}

// ---

var loaderShownFlags = new Array(true, true, true);

$(document).ready(function(){
    $('[data-toggle="popover"]').popover({html: true});
});

getBalance();

for(let level=1; level<=2; level++){
    getData(level);
}
