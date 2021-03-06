#pragma once

/**
 * Base class for composites embeddable in a VCV Widget
 * This is used for "real" implementations
 */
class WidgetComposite
{
public:
    WidgetComposite(Module * parent) :
        inputs(parent->inputs),
        outputs(parent->outputs),
        params(parent->params),
        lights(parent->lights),
        module(parent)
    {
    }
    virtual void step()
    {
    };
    float engineGetSampleRate()
    {
        return ::engineGetSampleRate();
    }
    float engineGetSampleTime()
    {
        return ::engineGetSampleTime();
    }
protected:
    std::vector<Input>& inputs;
    std::vector<Output>& outputs;
    std::vector<Param>& params;
    std::vector<Light>& lights;
private:
    Module * const module;
};
