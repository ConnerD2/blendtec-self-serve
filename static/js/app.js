var flavor = new Object();

flavor.blend = 3;
flavor.ice = 1;
flavor.water = 1;
flavor.flavor0 = ["flavor0",0];
flavor.flavor1 = ["flavor1",0];
flavor.flavor2 = ["flavor2",0];
flavor.flavor3 = ["flavor3",0];
flavor.flavor4 = ["flavor4",0];
flavor.flavor5 = ["flavor5",0];
flavor.flavor6 = ["flavor6",0];
flavor.flavor7 = ["flavor7",0];

function updateCup(){
    $("#ingredientsbody").html("");
    if(flavor.flavor0[1] > 0){
        $("#none").hide();
        $("#ingredientsbody").append(`
            <tr>
                <td>Banana</td>
                <td>${flavor.flavor0[1]}</td>
            </tr>
        `);
        //TODO: replace Banana with flavor from database
    }
    if(flavor.flavor1[1] > 0){
        $("#none").hide();
        $("#ingredientsbody").append(`
            <tr>
                <td>Blooming Berry</td>
                <td>${flavor.flavor1[1]}</td>
            </tr>
        `);
    }
    if(flavor.flavor2[1] > 0){
        $("#none").hide();
        $("#ingredientsbody").append(`
            <tr>
                <td>Blooming Berry</td>
                <td>${flavor.flavor2[1]}</td>
            </tr>
        `);
    }
    if(flavor.flavor3[1] > 0){
        $("#none").hide();
        $("#ingredientsbody").append(`
            <tr>
                <td>Strawberry</td>
                <td>${flavor.flavor3[1]}</td>
            </tr>
        `);
    }
    if(flavor.flavor4[1] > 0){
        $("#none").hide();
        $("#ingredientsbody").append(`
            <tr>
                <td>Pineapple</td>
                <td>${flavor.flavor4[1]}</td>
            </tr>
        `);
    }
    if(flavor.flavor5[1] > 0){
        $("#none").hide();
        $("#ingredientsbody").append(`
            <tr>
                <td>Blooming Berry</td>
                <td>${flavor.flavor5[1]}</td>
            </tr>
        `);
    }
    if(flavor.flavor6[1] > 0){
        $("#none").hide();
        $("#ingredientsbody").append(`
            <tr>
                <td>Harvest Greens</td>
                <td>${flavor.flavor6[1]}</td>
            </tr>
        `);
    }
    if(flavor.flavor7[1] > 0){
        $("#none").hide();
        $("#ingredientsbody").append(`
            <tr>
                <td>Tropical Harmony</td>
                <td>${flavor.flavor7[1]}</td>
            </tr>
        `);
    }
    //Check that if there are no selections, we set none back to visible
}

function resetrecipe(){
flavor.blend = 3;
flavor.ice = 1;
flavor.water = 1;
flavor.flavor0 = ["flavor0",0];
flavor.flavor1 = ["flavor1",0];
flavor.flavor2 = ["flavor2",0];
flavor.flavor3 = ["flavor3",0];
flavor.flavor4 = ["flavor4",0];
flavor.flavor5 = ["flavor5",0];
flavor.flavor6 = ["flavor6",0];
flavor.flavor7 = ["flavor7",0];
    updateCup();
}

function take(){
    $.get("/take");
}

function make() {
    setflavor();
    $.get("/make");
}
function rinse() {
    $.get("/rinse");
}
function cancel(){
    $.get("/cancel");
}
//Flavor Selection Handlers
function addBanana() {
    flavor.flavor0[1] += 1;
    updateCup();
}

function addWatermelon() {
    flavor.flavor1[1] += 1;
    updateCup();
}

function addMango() {
    flavor.flavor2[1] += 1;
    updateCup();
}

function addStrawberry() {
    flavor.flavor3[1] += 1;
    updateCup();
}

function addPineapple() {
    flavor.flavor4[1] += 1;
    updateCup();
}

function setBloomingBerry(){
    flavor.flavor1[1] += 1;
    updateCup();
}

function setHarvestGreens(){
    flavor.flavor6[1] += 1;
    updateCup();
}

function setSuperfruit(){
    flavor.flavor7[1] += 1;
    updateCup();
}

function setflavor(){
    //Populate from flavor selections
    $.post("/flavor", JSON.stringify(flavor));
}
function sethealth(health){
    $.post("/sethealth", JSON.stringify({'health': health}));
}

// These two accessible from admin page
function clean(){
    $.get("/clean");
}
function reset(){
    $.get("/reset");
}

function adminauth(){
    pw = $("").value
    actual = "Complex123";
    if(pw == actual){
        window.location.href = "/admin";
    }
    else {
        window.location.href = "/";
    }
}

$(document).ready(function () {
    if(window.location.href.indexOf("admin") > -1) {
        $.get( "/admin/wifi", function( datax ) {
            data = JSON.parse(datax);
            for (var i = 0, len = data.length; i < len; i++) {
                $('select').append(`<option value="${data[i]}">${data[i]}</option>`);
            }
            $('select').material_select();
        });
    }
});


document.addEventListener("DOMContentLoaded", function() {
    var lastElementClicked;
    var PrevLink = document.querySelector('a.prev');
    var NextLink = document.querySelector('a.next');

    Barba.Pjax.init();
    Barba.Prefetch.init();

    Barba.Dispatcher.on('linkClicked', function(el) {
        lastElementClicked = el;
    });

    var MovePage = Barba.BaseTransition.extend({
        start: function() {
            this.originalThumb = lastElementClicked;

            Promise
                .all([this.newContainerLoading, this.scrollTop()])
                .then(this.movePages.bind(this));
        },

        scrollTop: function() {
            var deferred = Barba.Utils.deferred();
            var obj = { y: window.pageYOffset };

            TweenLite.to(obj, 0.4, {
                y: 0,
                onUpdate: function() {
                    if (obj.y === 0) {
                        deferred.resolve();
                    }

                    window.scroll(0, obj.y);
                },
                onComplete: function() {
                    deferred.resolve();
                }
            });

            return deferred.promise;
        },

        movePages: function() {
            var _this = this;
            var goingForward = true;
            this.updateLinks();

            if (this.getNewPageFile() === this.oldContainer.dataset.prev) {
                goingForward = false;
            }

            TweenLite.set(this.newContainer, {
                visibility: 'visible',
                xPercent: goingForward ? 100 : -100,
                position: 'fixed',
                left: 0,
                top: 0,
                right: 0
            });

            TweenLite.to(this.oldContainer, 0.6, { xPercent: goingForward ? -100 : 100 });
            TweenLite.to(this.newContainer, 0.6, { xPercent: 0, onComplete: function() {
                TweenLite.set(_this.newContainer, { clearProps: 'all' });
                _this.done();
            }});
        },

        updateLinks: function() {
            PrevLink.href = this.newContainer.dataset.prev;
            NextLink.href = this.newContainer.dataset.next;
        },

        getNewPageFile: function() {
            return Barba.HistoryManager.currentStatus().url.split('/').pop();
        }
    });

    Barba.Pjax.getTransition = function() {
        return MovePage;
    };
});
