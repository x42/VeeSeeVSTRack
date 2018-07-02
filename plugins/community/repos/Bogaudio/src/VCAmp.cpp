
#include "VCAmp.hpp"
#include "mixer.hpp"

void VCAmp::onSampleRateChange() {
	float sampleRate = engineGetSampleRate();
	_levelSL.setParams(sampleRate, MixerChannel::levelSlewTimeMS, maxDecibels - minDecibels);
	_rms.setSampleRate(sampleRate);
}

void VCAmp::step() {
	if (inputs[IN_INPUT].active) {
		float level = params[LEVEL_PARAM].value;
		if (inputs[CV_INPUT].active) {
			level *= clamp(inputs[CV_INPUT].value, 0.0f, 10.0f) / 10.0f;
		}
		level *= maxDecibels - minDecibels;
		level += minDecibels;
		_amplifier.setLevel(_levelSL.next(level));
		outputs[OUT_OUTPUT].value = _saturator.next(_amplifier.next(inputs[IN_INPUT].value));
		_rmsLevel = _rms.next(outputs[OUT_OUTPUT].value / 5.0f);
	}
	else {
		_rmsLevel = _rms.next(0.0f);
	}
}

struct VCAmpWidget : ModuleWidget {
	static constexpr int hp = 3;

	VCAmpWidget(VCAmp* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(plugin, "res/VCAmp.svg")));
			addChild(panel);
		}

		addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto levelParamPosition = Vec(13.5, 18.0);

		auto cvInputPosition = Vec(10.5, 213.0);
		auto inInputPosition = Vec(10.5, 248.0);

		auto outOutputPosition = Vec(10.5, 286.0);
		// end generated by svg_widgets.rb

		auto slider = ParamWidget::create<VUSlider>(
			levelParamPosition,
			module,
			VCAmp::LEVEL_PARAM,
			0.0,
			1.0,
			abs(module->minDecibels) / (module->maxDecibels - module->minDecibels)
		);
		dynamic_cast<VUSlider*>(slider)->setVULevel(&(module->_rmsLevel));
		addParam(slider);

		addInput(Port::create<Port24>(cvInputPosition, Port::INPUT, module, VCAmp::CV_INPUT));
		addInput(Port::create<Port24>(inInputPosition, Port::INPUT, module, VCAmp::IN_INPUT));

		addOutput(Port::create<Port24>(outOutputPosition, Port::OUTPUT, module, VCAmp::OUT_OUTPUT));
	}
};

RACK_PLUGIN_MODEL_INIT(Bogaudio, VCAmp) {
   Model *modelVCAmp = createModel<VCAmp, VCAmpWidget>("Bogaudio-VCAmp", "VCAmp",  "amplifier", AMPLIFIER_TAG);
   return modelVCAmp;
}
