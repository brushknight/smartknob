#include "app.h"

#include "menu.h"
#include "music.h"
#include "climate.h"
#include "blinds.h"
#include "settings.h"
#include "light_dimmer.h"
#include "light_switch.h"
#include "3d_printer_chamber.h"

class AppsFactory
{
    App *construct(uint8_t position, char app_slug[48], char entity_id[128], char entity_name[32]);
};