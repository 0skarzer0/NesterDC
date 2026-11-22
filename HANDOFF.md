# HANDOFF - NesterDC + Special Edition

Estado a 2026-11-22. Para quien retome el trabajo (persona o IA).

## Qué es
Fork de pqrs-org/NesterDC 7.1 con `NesterDCSE/` (fuentes de NesterDC Special Edition 1.0, de Scherzo). `main` es la rama principal (7.1 + SE); `nester-7.1` conserva el 7.1 upstream (edab624). El propietario quiere compilar a partir de SE para NAOMI.

## Cómo se hizo
- El paquete `NesterDCSE-src_2005-12-24.zip` se extrajo con `unzip` en una carpeta nueva y se copió a `NesterDCSE/` (716 ficheros, 711 tras omitir 4 DLL y `dc_vmu.o`).
- El mensaje del commit lleva el SHA-256 del paquete.
- Los zips se descargaron de `files.dcemulation.org` pero el proxy de la sesión lo bloquea; los adjuntó el propietario.
- El 7.1 se comparó clonando `pqrs-org/NesterDC` (identidad por SHA-1 y similitud por nombre de fichero): ver README.

## Decisiones
- SE es la rama principal por decisión del propietario (2026-11-22): `main` avanzó (fast-forward, sin force) desde `special-edition`; el 7.1 queda en `nester-7.1`. La rama `special-edition` se conserva con el mismo contenido.
- SE se guarda tal cual en `NesterDCSE/` (no se fusiona con `nester/`): su estructura es plana y distinta.
- Se omitieron binarios; se conserva `cd/IP.BIN` (dato del CD).
- El zip «KOS» (KallistiOS 1.3) no se incluye: no es código de NesterDC.
- Sin licencia nueva ni releases de GitHub.

## Pendiente / ideas
- Port a NAOMI: partir de `NesterDCSE/new_nesterdcse`. Como en `mame4all-naomi`, requerirá KallistiOS actual con `KOS_SUBARCH=naomi` (SE usa KOS 1.3: habrá que portar las llamadas a la API), sin ROMs en el repo y documentándolo.
- Comprobar que SE compila (hace falta KallistiOS 1.3 y el toolchain `sh-elf` de la época) y documentar la toolchain.
- Ver cuánto de lo nuevo de SE (mappers, nesdb, zip) se podría llevar a `nester/` en `main`.
- Revisar la licencia de `libdreamscript/kjs`.

## Análisis del port a NAOMI (2026-11-22, solo leyendo el código; no se ha compilado nada)

SE son unas 31.000 líneas de C/C++. El núcleo del emulador es C portable; lo que depende de KallistiOS 1.3 y de la consola es poco y está localizado:

| Área | Qué usa SE | En NAOMI |
|---|---|---|
| Núcleo NES | `nes*.c/cpp`, `nes6502.c`, `nes_apu.c`, `nes_ppu.cpp`, `mapper/` (más de 150 mappers). Todo en C, sin ensamblador de CPU. | Se reutiliza tal cual. |
| Vídeo | API `pvr_*` (48 llamadas): `pvr_init`, escena/lista, `pvr_dr_target/commit` (cola de almacenamiento) y textura RGB565 256x256 (`main.cpp`). | KOS actual mantiene casi todo ese API y el PVR es el mismo. `mame4all-naomi` ya dibuja por el PVR con una textura. |
| Sonido | Driver ARM propio (`sound/`, con firmware `aica_fw.h` compilado con `DC_ARM_CC`) y `snd_sh4_to_aica*`, `spu_*`, `snd_stream_init`, `scherzo_snd_stream.c` (~40 llamadas). | Es lo más delicado: el firmware y su API son de KOS 1.3. Opción: reutilizar la salida de sonido de `mame4all-naomi` y alimentarla con el APU de Nester. |
| Mandos | `maple_enum_type`, `maple_dev_status`, `kbd_table_init` (pocas llamadas). | NAOMI no tiene maple: usar la entrada JVS de `mame4all-naomi`. |
| Ficheros | `fs_open/readdir/close`, módulo propio `fs_md.c` y `fs_handler_add` (VFS de KOS 1.3), explorador de ROMs, guardado en VMU (`dc_saving.cpp`). | Sin CD ni VMU: una ROM NES por imagen en el romdisk, como `mame4all-naomi` hace con cada juego. El explorador y el guardado en VMU no se portan al principio. |
| Menús y configuración | `libdreamscript` (JavaScript `kjs`) y los scripts `cd/*.js`. | No se portan al principio. |
| Utilidades | `zlib` (unzip propio), `png`, `timer_*`, `irq_set_handler`, `dbgio_printf`. | Disponibles en KOS actual (zlib/png en kos-ports). |
| Build | `Makefile` con `KOS_BASE/Makefile.rules` y `-D__DREAMCAST__`. | Un `Makefile.naomi` y `naomi/build.sh` al estilo de `mame4all-naomi`. |

**Plan propuesto (por orden):**
1. Compilar el núcleo (nes, apu, ppu, mappers) con gcc de escritorio con un arnés mínimo (sin pantalla): ejecuta una ROM de prueba unos fotogramas y compara el hash del fotograma. Así se comprueba que el núcleo funciona aislado, sin consola.
2. Capa NAOMI mínima: vídeo por PVR (textura), entrada JVS y ROM desde romdisk, sin sonido. Probar en Flycast.
3. Sonido: APU de Nester hacia la salida de sonido de `mame4all-naomi`.
4. Solo después, menús y guardado.

Riesgos: no hay toolchain KallistiOS en la sesión del asistente (compilar y probar lo hace el propietario); los mappers y el APU de SE pueden tener fallos propios (es una beta de 2005); la CPU del NES (6502 en C) a 200 MHz debería ir sobrada, pero no está medido.
