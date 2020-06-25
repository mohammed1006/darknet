#ifndef PARSER_H
#define PARSER_H
#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif
network parse_network_cfg(char *filename);
network parse_network_cfg_mem(char *str);
network parse_network_cfg_custom(char *filename, int batch, int time_steps);
network parse_network_cfg_custom_mem(char *str, int batch, int time_steps);
void save_network(network net, char *filename);
void save_weights(network net, char *filename);
void save_weights_upto(network net, char *filename, int cutoff);
void save_weights_double(network net, char *filename);
void load_weights(network *net, char *filename);
void load_weights_mem(network *net, char *data);
void load_weights_upto(network *net, char *filename, int cutoff);
void load_weights_upto_mem(network *net, char *data, int cutoff);

#ifdef __cplusplus
}
#endif
#endif
