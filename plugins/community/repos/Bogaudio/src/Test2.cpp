
#include <math.h>
#include <algorithm>

#include "Test2.hpp"
#include "pitch.hpp"

void Test2::onReset() {
}

void Test2::step() {
	if (!outputs[OUT_OUTPUT].active) {
		return;
	}

#ifdef COMPLEX_BIQUAD
	_complexBiquad.setComplexParams(
		params[PARAM1B_PARAM].value,
		params[PARAM2A_PARAM].value,
		params[PARAM2B_PARAM].value * M_PI,
		std::min(params[PARAM3A_PARAM].value, 0.9f),
		params[PARAM3B_PARAM].value * M_PI
	);
	float in = 0.0f;
	if (inputs[IN_INPUT].active) {
		in = inputs[IN_INPUT].value;
	}
	outputs[OUT_OUTPUT].value = _complexBiquad.next(in);

#elif MULTIPOLE
	++_steps;
	if (_steps >= maxSteps) {
		_steps = 0;

		_filter.setParams(
			params[PARAM2A_PARAM].value <= 0.5f ? MultipoleFilter::LP_TYPE : MultipoleFilter::HP_TYPE,
			2 * clamp((int)(params[PARAM1B_PARAM].value * (MultipoleFilter::maxPoles / 2)), 1, MultipoleFilter::maxPoles / 2),
			engineGetSampleRate(),
			params[PARAM1A_PARAM].value * engineGetSampleRate() / 2.0f,
			params[PARAM2B_PARAM].value * MultipoleFilter::maxRipple
		);
		// _filter.setParams(
		// 	MultipoleFilter::HP_TYPE,
		// 	4,
		// 	engineGetSampleRate(),
		// 	0.1f * engineGetSampleRate(),
		// 	0.1f
		// );
	}
	float in = 0.0f;
	if (inputs[IN_INPUT].active) {
		in = inputs[IN_INPUT].value;
	}
	outputs[OUT_OUTPUT].value = _filter.next(in);

#elif ADSR_ENVELOPE
  if (outputs[OUT_OUTPUT].active) {
		_adsr.setSampleRate(engineGetSampleRate());
		if (inputs[IN_INPUT].active) {
			_trigger.process(inputs[IN_INPUT].value);
		}
		_adsr.setGate(_trigger.isHigh());
		_adsr.setAttack(powf(params[PARAM1A_PARAM].value, 2.0f) * 10.0f);
		_adsr.setDecay(powf(params[PARAM1B_PARAM].value, 2.0f) * 10.0f);
		_adsr.setSustain(params[PARAM2A_PARAM].value);
		_adsr.setRelease(powf(params[PARAM2B_PARAM].value, 2.0f) * 10.0f);
		float attackShape = params[PARAM3A_PARAM].value;
		if (attackShape < 0.5f) {
			attackShape += 0.5f;
		}
		else {
			attackShape -= 0.5;
			attackShape *= 2.0f;
			attackShape += 1.0f;
		}
		float decayShape = params[PARAM3B_PARAM].value;
		if (decayShape < 0.5f) {
			decayShape += 0.5f;
		}
		else {
			decayShape -= 0.5;
			decayShape *= 2.0f;
			decayShape += 1.0f;
		}
		_adsr.setShapes(attackShape, decayShape, decayShape);
		outputs[OUT_OUTPUT].value = _adsr.next() * 10.0f;
	}

#elif LIMITER
	float shape = params[PARAM1A_PARAM].value * 5.0f;
	float knee = params[PARAM2A_PARAM].value * 10.0f;
	float limit = params[PARAM2B_PARAM].value * 15.0f;
	float scale = params[PARAM1B_PARAM].value * 2.0f + 1.0f;
	_limiter.setParams(shape, knee, limit, scale);
	outputs[OUT_OUTPUT].value = _limiter.next(inputs[IN_INPUT].value);
#endif
}

// float Test2::oscillatorPitch1A() {
// 	if (inputs[CV1A_INPUT].active) {
// 		return cvToFrequency(inputs[CV1A_INPUT].value);
// 	}
// 	return 10000.0 * powf(params[PARAM1_PARAM].value, 2.0);
// }


struct Test2Widget : ModuleWidget {
	Test2Widget(Test2* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * 6, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(plugin, "res/Test2.svg")));
			addChild(panel);
		}

		addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto param1aParamPosition = Vec(9.5, 38.5);
		auto param2aParamPosition = Vec(9.5, 138.5);
		auto param3aParamPosition = Vec(9.5, 238.5);
		auto param1bParamPosition = Vec(54.5, 38.5);
		auto param2bParamPosition = Vec(54.5, 138.5);
		auto param3bParamPosition = Vec(54.5, 238.5);

		auto cv1aInputPosition = Vec(10.5, 78.0);
		auto cv2aInputPosition = Vec(10.5, 178.0);
		auto cv3aInputPosition = Vec(10.5, 278.0);
		auto cv1bInputPosition = Vec(55.5, 78.0);
		auto cv2bInputPosition = Vec(55.5, 178.0);
		auto cv3bInputPosition = Vec(55.5, 278.0);
		auto inInputPosition = Vec(10.5, 323.0);

		auto outOutputPosition = Vec(55.5, 323.0);
		// end generated by svg_widgets.rb

		addParam(ParamWidget::create<Knob26>(param1aParamPosition, module, Test2::PARAM1A_PARAM, 0.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob26>(param2aParamPosition, module, Test2::PARAM2A_PARAM, 0.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob26>(param3aParamPosition, module, Test2::PARAM3A_PARAM, 0.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob26>(param1bParamPosition, module, Test2::PARAM1B_PARAM, 0.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob26>(param2bParamPosition, module, Test2::PARAM2B_PARAM, 0.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob26>(param3bParamPosition, module, Test2::PARAM3B_PARAM, 0.0, 1.0, 0.0));

		addInput(Port::create<Port24>(cv1aInputPosition, Port::INPUT, module, Test2::CV1A_INPUT));
		addInput(Port::create<Port24>(cv2aInputPosition, Port::INPUT, module, Test2::CV2A_INPUT));
		addInput(Port::create<Port24>(cv3aInputPosition, Port::INPUT, module, Test2::CV3A_INPUT));
		addInput(Port::create<Port24>(cv1bInputPosition, Port::INPUT, module, Test2::CV1B_INPUT));
		addInput(Port::create<Port24>(cv2bInputPosition, Port::INPUT, module, Test2::CV2B_INPUT));
		addInput(Port::create<Port24>(cv3bInputPosition, Port::INPUT, module, Test2::CV3B_INPUT));
		addInput(Port::create<Port24>(inInputPosition, Port::INPUT, module, Test2::IN_INPUT));

		addOutput(Port::create<Port24>(outOutputPosition, Port::OUTPUT, module, Test2::OUT_OUTPUT));
	}
};

RACK_PLUGIN_MODEL_INIT(Bogaudio, Test2) {
   Model *modelTest2 = Model::create<Test2, Test2Widget>("Bogaudio", "Bogaudio-Test2", "Test2");
   return modelTest2;
}
