#include "ESeries.hpp"


RACK_PLUGIN_MODEL_DECLARE(ESeries, E340);

RACK_PLUGIN_INIT(ESeries) {
   RACK_PLUGIN_INIT_ID();

	RACK_PLUGIN_MODEL_ADD(ESeries, E340);
}
