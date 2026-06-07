# ReadyToRead

A Pebble watchface designed for easy readability — large, clear text for people who want to glance at their watch and immediately know the time and date.

| Aplite | Basalt | Chalk |
|:---:|:---:|:---:|
| ![Aplite](/screenshots/readytoread~aplite.png) | ![Basalt](/screenshots/readytoread~basalt.png) | ![Chalk](/screenshots/readytoread~chalk.png) |
| Pebble Classic / Steel | Pebble Time | Pebble Time Round |

| Diorite | Emery |
|:---:|:---:|
| ![Diorite](/screenshots/readytoread~diorite.png) | ![Emery](/screenshots/readytoread~emery.png) |
| Pebble 2 | Pebble Time 2 |

## Features

- **Date** — Month and day in large text (e.g. `JUN 07`)
- **Time** — 24-hour format in the largest available font (e.g. `18:30`)
- **Day of week** — Color-coded for quick recognition
  - Sunday → red
  - Saturday → blue
  - Weekdays → white
- **Battery** — Displayed as a percentage; turns red at 20% or below, shows `+` prefix while charging
- Compatible with all 5 Pebble platforms: aplite, basalt, chalk, diorite, emery

## Supported Platforms

| Platform | Device | Display |
|---|---|---|
| aplite | Pebble Classic / Steel | 144×168, monochrome |
| basalt | Pebble Time | 144×168, color |
| chalk | Pebble Time Round | 180×180, color, round |
| diorite | Pebble 2 | 144×168, monochrome |
| emery | Pebble Time 2 | 200×228, color |

## Build & Install

**Requirements**
- [Pebble SDK](https://developer.rebble.io/) 4.x
- `pebble` CLI tool

**Build**
```sh
pebble build
```

**Install on emulator**
```sh
pebble install --emulator basalt
```

**Install on device**
```sh
pebble install --phone <phone-ip>
```

## Design Notes

ReadyToRead prioritizes legibility over decoration. Font sizes are determined at runtime using the actual font metrics (`graphics_text_layout_get_content_size`) so the layout adapts correctly across all screen sizes.

- **Date row** uses `BITHAM_30_BLACK` — wide, heavy strokes that are easy to read at a glance
- **Time row** uses `ROBOTO_BOLD_SUBSET_49` — the largest digit-only system font available
- **Battery / day of week** use `GOTHIC_28_BOLD` — consistent weight for secondary information

## License

MIT License. See [LICENSE](LICENSE) for details.

Based on the original [Concentricity](https://github.com/pebble-examples/concentricity) watchface example by Pebble Technology / Pebble Hacks.
