//! Lighying subsystem object 
var LightingSubsystem = {
    // Members
    currentLightIntensityGauge: undefined,
    desiredLightIntensityGauge: undefined,
    lightSensorRegisterValueGauge: undefined,
    lampSupplyRegisterValueGauge: undefined,
    manualLampSupplyLevelCheckbox: undefined,
    manualLampSupplyLevelSlider: undefined,
    manualLampSupplyLevelLabel: undefined,
    // Functions
    init: function() {
        LightingSubsystem.currentLightIntensityGauge = new JustGage({
            id: "CurrentLightIntensityGauge",
            value: 100.0,
            min: 0,
            max: 100,
            title: "Current light intensity",
            label: "%",
            levelColors: ["#ff0000", "#EAFF00", "#00F00"],
            titleFontColor: "black",
            gaugeColor: "white",
            showInnerShadow: true,
            shadowOpacity: 1,
            shadowSize: 5,
            shadowVerticalOffset: -1,
            labelFontColor: "black",
            titleFontColor: "black"
        });
        LightingSubsystem.desiredLightIntensityGauge = new JustGage({
            id: "DesiredLightIntensityGauge",
            value: 50.0,
            min: 0,
            max: 100,
            title: "Desired light intensity",
            label: "%",
            levelColors: ["#ff0000", "#EAFF00", "#00F00"],
            titleFontColor: "black",
            gaugeColor: "white",
            showInnerShadow: true,
            shadowOpacity: 1,
            shadowSize: 5,
            shadowVerticalOffset: -1,
            labelFontColor: "black",
            titleFontColor: "black"
        });
        LightingSubsystem.lightSensorRegisterValueGauge = new JustGage({
            id: "LightSensorRegisterValueGauge",
            value: 0,
            min: 0,
            max: 255,
            title: "Light sensor level",
            levelColors: ["#ff0000", "#EAFF00", "#00F00"],
            titleFontColor: "black",
            gaugeColor: "white",
            showInnerShadow: true,
            shadowOpacity: 1,
            shadowSize: 5,
            shadowVerticalOffset: -1,
            labelFontColor: "black",
            titleFontColor: "black"
        });
        LightingSubsystem.lampSupplyRegisterValueGauge = new JustGage({
            id: "LampSupplyRegisterValueGauge",
            value: 0,
            min: 0,
            max: 255,
            title: "Lamp supply level",
            levelColors: ["#ff0000", "#EAFF00", "#00F00"],
            titleFontColor: "black",
            gaugeColor: "white",
            showInnerShadow: true,
            shadowOpacity: 1,
            shadowSize: 5,
            shadowVerticalOffset: -1,
            labelFontColor: "black",
            titleFontColor: "black"
        });
        LightingSubsystem.manualLampSupplyLevelCheckbox = $("#ManualLampSupplyLevelCheckbox");
        LightingSubsystem.manualLampSupplyLevelLabel = $("#ManualLampSupplyLevelLabel");
        LightingSubsystem.manualLampSupplyLevelSlider = $("#ManualLampSupplyLevelSlider").slider({
             min: 0,
             max: 255,
             slide: function(event, ui) {
                 LightingSubsystem.manualLampSupplyLevelLabel.html(ui.value);
                 LightingSubsystem.setManualMode(true, ui.value);
                 //console.dir(ui);
             }
        });

        // Setting viewBox for svg gauges
        var svgs = $("[id$=Gauge] svg");
        var width = $(window).width();
        var height = $(window).height();
        var wvb;
        var hvb;
        var sxvb;
        var syvb;
        // firefox
        if (typeof InstallTrigger !== 'undefined') {
            var wp = (width - 648) / (1580 - 648);
            var hp = (height - 493) / (887 - 493);
            wvb = wp * (480 - 310) + 310;
            hvb = hp * (380 - 180) + 180;
            sxvb = wp * 160;
            syvb = 25;
        } else {
            var wp = (width - 584) / (1580 - 584);
            var hp = (height - 378) / (1019 - 378);
            wvb = wp * (450 - 134) + 134;
            hvb = hp * (350 - 120) + 120;
            sxvb = wp * (180 - 50) + 50;
            syvb = 15;
        }
        for (var i=0; i < svgs.length; i++) {
            if (typeof InstallTrigger !== 'undefined') {
                svgs[i].setAttribute("viewBox", sxvb + " 10 " + wvb + " "+ hvb);
            } else {
                svgs[i].setAttribute("viewBox", sxvb + " 20 " + wvb + " "+ hvb);
            }
        }

        // Using checkbox
        // LightingSubsystem.manualLampSupplyLevelCheckbox.change(function(eventObject) {
        //     var manual = false;
        //     manual = $(this).prop("checked");
        //     console.log("Manual mode changed: " + manual);
        //     LightingSubsystem.manualLampSupplyLevelSlider.slider(manual?"enable":"disable");
        //     var level = LightingSubsystem.manualLampSupplyLevelSlider.slider("value");
        //     LightingSubsystem.setManualMode(manual, level);
        // });
        
        // Using button
        LightingSubsystem.manualLampSupplyLevelCheckbox.on('click', function() {
            var manual = false;
            // Set state
            if(LightingSubsystem.manualLampSupplyLevelCheckbox.hasClass('active')) {
                LightingSubsystem.manualLampSupplyLevelCheckbox.removeClass('active');
                manual = false;
            } else {
                LightingSubsystem.manualLampSupplyLevelCheckbox.addClass('active');
                manual = true;
            }
            console.log("Manual mode changed: " + manual);
            LightingSubsystem.manualLampSupplyLevelSlider.slider(manual ? "enable" : "disable");
            var level = LightingSubsystem.manualLampSupplyLevelSlider.slider("value");
            LightingSubsystem.setManualMode(manual, level);
        });
        LightingSubsystem.manualLampSupplyLevelSlider.slider("disable");
        // Fetching subsystem status and scheduling a respective periodic job
        LightingSubsystem.fetchStatus();
        window.setInterval(LightingSubsystem.fetchStatus, 1000);
    },
    fetchStatus: function() {
        $.ajax("lighting", {
            error: function(jqXHR, textStatus, errorThrown) {
                console.log("AJAX request error: ", textStatus, errorThrown);
            },
            success: function(data, textStatus, jqXHR) {
                LightingSubsystem.currentLightIntensityGauge.refresh((Math.round(data.currentLightIntensity * 10) / 10).toFixed(1));
                LightingSubsystem.desiredLightIntensityGauge.refresh((Math.round(data.desiredLightIntensity * 10) / 10).toFixed(1));
                LightingSubsystem.lightSensorRegisterValueGauge.refresh(data.lightSensorRegisterValue);
                LightingSubsystem.lampSupplyRegisterValueGauge.refresh(data.lampSupplyLevelRegisterValue);
                LightingSubsystem.manualLampSupplyLevelSlider.slider("value", data.lampSupplyLevelRegisterValue);
                LightingSubsystem.manualLampSupplyLevelLabel.html(data.lampSupplyLevelRegisterValue);
                // LightingSubsystem.manualLampSupplyLevelCheckbox.prop("checked", data.isManualLampSupplyLevel);
                if (data.isManualLampSupplyLevel) {
                    LightingSubsystem.manualLampSupplyLevelCheckbox.addClass('active');
                } else {
                    LightingSubsystem.manualLampSupplyLevelCheckbox.removeClass('active');
                }
                LightingSubsystem.manualLampSupplyLevelSlider.slider(data.isManualLampSupplyLevel ? "enable" : "disable");
            }
        });
        
    },
    setManualMode: function(mode, level) {
        if (mode) {
            $.ajax("lighting?mode=set_manual_lamp_supply&level=" + level, {
                error: function(jqXHR, textStatus, errorThrown) {
                    console.log("AJAX request error: ", textStatus, errorThrown);
                    console.log("set manual mode: error");
                },
                success: function(data, textStatus, jqXHR) {
                    console.log("set manual mode: success");
                }
            });    
        } else {
            $.ajax("lighting?mode=reset_manual_lamp_supply", {
                error: function(jqXHR, textStatus, errorThrown) {
                    console.log("AJAX request error: ", textStatus, errorThrown);
                    console.log("set manual mode: error");
                },
                success: function(data, textStatus, jqXHR) {
                    console.log("set manual mode: success");
                }
            });   
        }
    }
};

$(document).ready(function() {
    LightingSubsystem.init();
});
