# TRADUCIR A INGLES!!!!!!!!!!!!
# PYTHIA8 NOTES
## 4 Partes 
INIT (pre simulacion y solo tiene show info y plugins)
NEXT (during)
STAT (post)
MAIN (ESTA VA TOTALMENTE APARTE)

## Function types
| Tipo | Data type | Usos |
|---|---|---|
| Flag | bool | activar/desactivar algo  (on/off)|
| Mode | int | elegir entre opciones discretas (menu options)|
| Parm | double | ajustar valores continuos |


## 🔧 1. Initialization settings (`Init:`)

| Tipo  | Setting                          | Default | Qué hace realmente                                              |
|-------|----------------------------------|---------|-----------------------------------------------------------------|
| flag  | Init:showProcesses               | on      | Lista los procesos activos y sus secciones eficaces estimadas   |
| flag  | Init:showMultipartonInteractions | on      | Muestra info de inicialización del modelo de MPI               |
| flag  | Init:showChangedSettings         | on      | Imprime los parámetros modificados respecto al default          |
| flag  | Init:showAllSettings             | off     | Imprime todos los settings (muy largo)                          |
| flag  | Init:showChangedParticleData     | on      | Muestra cambios en datos de partículas                          |
| flag  | Init:showChangedResonanceData    | off     | Incluye resonancias inicializadas (puede ser muy largo)         |
| flag  | Init:showAllParticleData         | off     | Lista todas las partículas y decays                             |
| mode  | Init:showOneParticleData         | 0       | Muestra info de una partícula concreta (por PDG id)             |
| wvec  | Init:plugins                     | {}      | Carga plugins externos (PDFs, modelos, etc.)                    |


## ⚙️ 2. Event-generation settings (`Next:`)

| Tipo  | Setting                          | Default | Qué hace realmente                                       |
|-------|----------------------------------|---------|----------------------------------------------------------|
| mode  | Next:numberCount                 | 1000    | Imprime progreso cada N eventos                          |
| mode  | Next:numberShowLHA               | 1       | Muestra info de entrada Les Houches                      |
| mode  | Next:numberShowInfo              | 1       | Muestra objeto `Info` (evento)                           |
| mode  | Next:numberShowProcess           | 1       | Muestra el proceso duro                                  |
| mode  | Next:numberShowEvent             | 1       | Muestra el evento completo                               |
| flag  | Next:showScaleAndVertex          | off     | Añade escala, polarización y vértice                     |
| flag  | Next:showMothersAndDaughters     | off     | Añade genealogía completa de partículas                  |


## 📊 3. Statistics (`Stat:`)

| Tipo  | Setting                  | Default | Qué hace realmente                              |
|-------|--------------------------|---------|-------------------------------------------------|
| flag  | Stat:showProcessLevel    | on      | Estadísticas de eventos y cross sections        |
| flag  | Stat:showPartonLevel     | off     | Estadísticas de MPI                             |
| flag  | Stat:showErrors          | on      | Errores, aborts y warnings                      |
| flag  | Stat:reset               | off     | Resetea contadores al llamar `stat()`           |


---
---
---


# Convertidor Pythia → HepMC3

## Inicialización
```cpp
Pythia8ToHepMC3 toHepMC;
```

---

## Opciones (`set_*`)

| Método                          | Valor por defecto | Qué hace                                                                                  |
|---------------------------------|:-----------------:|-------------------------------------------------------------------------------------------|
| `set_print_inconsistency()`     | `true`            | Avisa si hay inconsistencias en relaciones madre-hija (errores en la estructura del evento) |
| `set_free_parton_warnings()`    | `true`            | Lanza un warning si hay partones libres (quarks/gluones sin hadronizar)                   |
| `set_crash_on_problem()`        | `false`           | Si hay partones libres y está activo → el programa aborta (útil para debug estricto)      |
| `set_convert_gluon_to_0()`      | `false`           | Cambia el ID del gluón en metadatos de PDFs (21 → 0). No afecta al evento físico         |
| `set_store_pdf()`               | `true`            | Guarda info de PDFs por evento: sabores iniciales, x₁/x₂, escala Q, valores xf(x,Q)     |
| `set_store_proc()`              | `true`            | Guarda info del proceso duro: código de proceso, escala de renormalización, αs, αem       |
| `set_store_xsec()`              | `true`            | Guarda sección eficaz y peso del evento: cross section (pb), error, peso                 |

---

## Uso principal
```cpp
HepMC3::GenEvent evt;       // Crear evento HepMC
if (!pythia.next()) return; // Generar evento en Pythia

toHepMC.fill_next_event(pythia, &evt); // Convertir Pythia → HepMC
```

---

## Resultado

El objeto `evt` contiene, tras la conversión:

- Partículas y vértices
- Relaciones madre-hija
- Cuadrimomentos
- *(Opcional)* PDFs, proceso duro y cross section

El objeto `evt` ya está listo para:

- Guardarse en fichero HepMC
- Pasarse a Geant4
- Usarse directamente en análisis