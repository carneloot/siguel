#include <model/comando.r>
#include <control/controlador.r>

#include <string.h>
#include <stdlib.h>

#include <model/elemento.h>
#include <model/utils.h>

/**
 * Comando: c[qhts]
 * Params:  cstrok cfill
 * Cores do preenchimento e da borda dos (as)
 * quadras/hidrantes/semaforos/torres de celular
 */
static int comando_c_all(void *_this, void *_controlador, int elemento_id) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  if (controlador->cores[elemento_id])
    free(controlador->cores[elemento_id]);
  if (controlador->cores_borda[elemento_id])
    free(controlador->cores_borda[elemento_id]);

  controlador->cores[elemento_id] =
    (char *) malloc((strlen(params[1]) + 1) * sizeof(char));
  strcpy(controlador->cores[elemento_id], params[1]);

  controlador->cores_borda[elemento_id] =
    (char *) malloc((strlen(params[0]) + 1) * sizeof(char));
  strcpy(controlador->cores_borda[elemento_id], params[0]);

  return 1;
}

int comando_cq(void *this, void *controlador) {
  return comando_c_all(this, controlador, QUADRA);
}

int comando_ch(void *this, void *controlador) {
  return comando_c_all(this, controlador, HIDRANTE);
}

int comando_ct(void *this, void *controlador) {
  return comando_c_all(this, controlador, RADIO_BASE);
}

int comando_cs(void *this, void *controlador) {
  return comando_c_all(this, controlador, SEMAFORO);
}