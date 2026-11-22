# NesterDC (fork) + NesterDC Special Edition

Fork de [pqrs-org/NesterDC](https://github.com/pqrs-org/NesterDC) (emulador de NES para Dreamcast, versión 7.1) al que se ha añadido el código fuente de **NesterDC Special Edition 1.0**, de Scherzo.

> Archivo de preservación. Los commits **no son de sus autores**: reconstruyen paquetes de fuentes publicados.

## Ramas

| Rama | Contenido |
|---|---|
| `main` | NesterDC 7.1 de pqrs-org, tal cual (sin tocar). El `README` original sigue ahí. |
| `special-edition` | `main` + la carpeta `NesterDCSE/` con los fuentes de NesterDC SE 1.0, y esta documentación. |

## `NesterDCSE/`

Contenido del paquete `NesterDCSE-src_2005-12-24.zip`, sin cambios salvo los binarios omitidos:

- `new_nesterdcse/` – el emulador (en una sola carpeta, a diferencia de `nester/` del 7.1), con `cd/` (datos y scripts `.js` del CD: `IP.BIN`, fuentes, menús).
- `libdreamscript/` – motor de JavaScript (`kjs`) que usa SE para los menús y la configuración.
- `NesterDCSEBuilderWx/` – herramienta de Windows (wxWidgets) para montar el CD.

Omitidos por ser binarios: 4 DLL de wxWidgets/MinGW (`NesterDCSEBuilderWx`) y `new_nesterdcse/dc_vmu.o`.

## Relación con NesterDC 7.1

Comparando `NesterDCSE/new_nesterdcse` con `nester/` del 7.1: de 410 ficheros de código, 250 son idénticos byte a byte, 77 están modificados y 83 son nuevos (más mappers, base de datos `nesdb`, soporte de zip, ratón, explorador de ficheros, visualización). Del 7.1 no están en SE 35 de 356 ficheros de código. Es una evolución del 7.1; no se ha comprobado que sea exactamente esa versión. Para ver los cambios:

```sh
git diff --no-index nester NesterDCSE/new_nesterdcse   # o: git diff main:nester special-edition:NesterDCSE/new_nesterdcse
```

## Qué no está

- El segundo paquete publicado, `NesterDCSE-KOS-src_2005-12-24.zip`, **no es de NesterDC**: es el SDK KallistiOS 1.3 con `kos-ports` (SDL, freetype). No se ha incluido. SHA-256: `64b30a6b7e586943c4617cba0f0ed01012d9658acb87cd5784960ff8db0752bb`. El KallistiOS actual está en [KallistiOS](https://github.com/KallistiOS/KallistiOS).
- La versión 1.1 no oficial (Christuserloeser): no hay fuentes publicadas.
- No se ha compilado nada.

## Créditos

- **pqrs-org** (Takayama Fumihiko y colaboradores) – NesterDC; y el **proyecto Nester** original en el que se basa, con las aportaciones de NNNesterJ (`README.nnnesterj`).
- **Scherzo** – NesterDC Special Edition.
- **Christuserloeser** – actualización no oficial 1.1.
- **dcemulation.org** – alojó los paquetes originales.
- **KallistiOS** – SDK de Dreamcast.

## Licencia

No se añade ninguna. Se conservan `COPYING` (NesterDC, GPL) y `NesterDCSE/new_nesterdcse/cd/copying.txt`, y las cabeceras de cada fichero. `libdreamscript/kjs` puede tener licencia propia; revísalo antes de reutilizarlo.
