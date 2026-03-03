/** \file components.c
 *
 * Available Components and related utilities
 */
#include "components.h"
#include "src/settings/definitions.h"
#include <stdlib.h>

LSComponent* ls_component_title_new(void);
LSComponent* ls_component_splits_new(void);
LSComponent* ls_component_timer_new(void);
LSComponent* ls_component_detailed_timer_new(void);
LSComponent* ls_component_prev_segment_new(void);
LSComponent* ls_component_best_sum_new(void);
LSComponent* ls_component_pb_new(void);
LSComponent* ls_component_wr_new(void);
LSComponent* ls_component_comparison_new(void);

static LSComponentAvailable ls_available_components[] = {
    { "title", ls_component_title_new },
    { "splits", ls_component_splits_new },
    { "timer", ls_component_timer_new },
    { "detailed_timer", ls_component_detailed_timer_new },
    { "prev_segment", ls_component_prev_segment_new },
    { "best_sum", ls_component_best_sum_new },
    { "comparison", ls_component_comparison_new },
    { "pb", ls_component_pb_new },
    { "wr", ls_component_wr_new },
    { NULL, NULL }
};

/**
 * Initializes the array of active components
 * by reading the user settings
 *
 * @param[out] ls_components The Array of Activated Components
 */
void initialize_components(LSComponentAvailable** ls_components)
{
    // Enumerate the active components
    ComponentsConfig components = cfg.components;
    int active_components_count = 0;
    for (size_t i = 0; i < sizeof(components) / sizeof(ConfigEntry); i++) {
        ConfigEntry entry = ((ConfigEntry*)&components)[i];
        if (entry.value.b) {
            active_components_count++;
        }
    }
    *ls_components = malloc((active_components_count + 1) * sizeof(LSComponentAvailable));
    if (!*ls_components) {
        printf("Cannot allocate memory for components");
        abort();
    }
    // Create the fitted, null-terminated ls_components array, containing the active components
    int compindex = 0;
    for (size_t i = 0; i < sizeof(components) / sizeof(ConfigEntry); i++) {
        ConfigEntry entry = ((ConfigEntry*)&components)[i];
        for (size_t j = 0; ls_available_components[j].name != NULL; j++) {
            if (strcmp(ls_available_components[j].name, entry.key) == 0) {
                if (entry.value.b) {
                    (*ls_components)[compindex] = ls_available_components[j];
                    compindex++;
                }
                break;
            }
        }
    }
    (*ls_components)[compindex].name = NULL;
    (*ls_components)[compindex].new = NULL;
}
