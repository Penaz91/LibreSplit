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

LSComponentAvailable ls_available_components[] = {
    { "title", ls_component_title_new },
    { "splits", ls_component_splits_new },
    { "timer", ls_component_timer_new },
    { "detailed_timer", ls_component_detailed_timer_new },
    { "prev_segment", ls_component_prev_segment_new },
    { "best_sum", ls_component_best_sum_new },
    { "pb", ls_component_pb_new },
    { "wr", ls_component_wr_new },
    { NULL, NULL }
};

void initialize_components()
{
    SectionInfo* section;
    for (size_t i = 0; i < sections_count; i++) {
        section = (SectionInfo*)&sections[i];
        if (strcmp(section->name, "components") == 0) {
            break;
        }
    }
    int components_count = 0;
    for (size_t i = 0; i < sizeof(section->count); i++) {
        ConfigEntry entry = ((ConfigEntry*)section->entries)[i];
        if (entry.value.b) {
            components_count++;
        }
    }
    ls_components = malloc(components_count + 1 * sizeof(LSComponentAvailable));
    int compindex = 0;
    for (size_t i = 0; i < sizeof(section->count); i++) {
        ConfigEntry entry = ((ConfigEntry*)section->entries)[i];
        for (size_t j = 0; j < sizeof(ls_available_components); j++) {
            if (strcmp(ls_available_components[j].name, entry.key) == 0) {
                ls_components[compindex] = ls_available_components[j];
                compindex++;
            }
        }
    }
    ls_components[compindex].name = NULL;
    ls_components[compindex].new = NULL;
}

void deinitialize_components()
{
    free(ls_components);
}
