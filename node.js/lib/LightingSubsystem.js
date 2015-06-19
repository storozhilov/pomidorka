// Lighting subsystem
//
// Requires 'strftime' NPM module. Installation: 'npm install strftime'

var strftime = require('strftime');

var Settings = {
	MAX_LUMINOSITY_VALUE: 255,
	MAX_LAMP_SUPPLY_VALUE: 255,
	LAMP_SUPPLY_INCREMENT: 5,

	CHECK_STATE_DELAY_MS: 1000,
	ADJUSTMENT_DELAY_MS: 25
};

module.exports = {
	LightingSubsystem: function(ctx, statusReg, lightSensorReg, lampSupplyReg) {
		// Composing properties
		this.ctx = ctx;
		this.statusReg = statusReg;
		this.lightSensorReg = lightSensorReg;
		this.lampSupplyReg = lampSupplyReg;
		this.endpointStatus = 0;
		this.luminosity = 0;
		this.lampSupply = 0;
		this.desiredLuminosity = 255;

		// Composing methods
		this.makeStep = function() {
			// Getting current data from the endpoint
			regs = this.ctx.getInputRegs(0, this.getMaxReg() + 1);
			this.endpointStatus = regs[this.statusReg];
			this.luminosity = regs[this.lightSensorReg];
			this.lampSupply = regs[this.lampSupplyReg];
			console.log("[%s]: endpoint status: %d, actual luminosity: %d, desired luminosity: %d, lamp supply: %d",
					strftime('%F %T.%L'), this.endpointStatus, this.luminosity, this.desiredLuminosity, 
					this.lampSupply);

			// Making ajustment to lamp supply and scheduling next job
			if (this.luminosity < this.desiredLuminosity) {
				if (this.lampSupply < Settings.MAX_LAMP_SUPPLY_VALUE) {
					var newLampSupply = Math.min(this.lampSupply + Settings.LAMP_SUPPLY_INCREMENT,
							Settings.MAX_LAMP_SUPPLY_VALUE);
					console.log("[%s]: Incrementing lamp supply value from %d to %d", 
							strftime('%F %T.%L'), this.lampSupply, newLampSupply);
					this.ctx.setReg(this.lampSupplyReg, newLampSupply);
					setTimeout(arguments.callee.bind(this), Settings.ADJUSTMENT_DELAY_MS);
				} else {
					console.log("[%s]: Lamp supply is already on maximum value - nothing to do",
							strftime('%F %T.%L'));
					setTimeout(arguments.callee.bind(this), Settings.CHECK_STATE_DELAY_MS);
				}
			} else if (this.luminosity > this.desiredLuminosity) {
				if (this.lampSupply > 0) {
					var newLampSupply = Math.max(this.lampSupply - Settings.LAMP_SUPPLY_INCREMENT, 0);
					console.log("[%s]: Decrementing lamp supply value from %d to %d", 
							strftime('%F %T.%L'), this.lampSupply, newLampSupply);
					this.ctx.setReg(this.lampSupplyReg, newLampSupply);
					setTimeout(arguments.callee.bind(this), Settings.ADJUSTMENT_DELAY_MS);
				} else {
					console.log("[%s]: Lamp supply is already on minimum value - nothing to do",
							strftime('%F %T.%L'));
					setTimeout(arguments.callee.bind(this), Settings.CHECK_STATE_DELAY_MS);
				}
			} else {

				setTimeout(arguments.callee.bind(this), Settings.CHECK_STATE_DELAY_MS);
			}
		};
		this.setDesiredLuminosity(newValue) {
			this.desiredLuminosity = newValue;
		}
		this.setSupply = function(val) {
			// TODO
		};
		this.getMaxReg = function() {
			return Math.max(this.statusReg, this.lightSensorReg, this.lampSupplyReg);
		};

		this.makeStep();
		return this;
	}
};
