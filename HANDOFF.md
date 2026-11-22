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
