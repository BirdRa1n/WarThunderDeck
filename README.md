# WarThunderDeck

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![PlatformIO](https://img.shields.io/badge/built%20with-PlatformIO-orange)](https://platformio.org)
[![ESP32](https://img.shields.io/badge/MCU-ESP32-blue)](https://www.espressif.com/en/products/socs/esp32)

> **An open-source auxiliary instrument panel for War Thunder — powered by the ESP32 "Cheap Yellow Display" (CYD).**

<!-- TODO: add a photo/GIF of the device in action here -->
<!-- ![WarThunderDeck in action](docs/demo.gif) -->

---

## English

### Features

- **Real-time telemetry** — speed, altitude, heading, RPM, G-force, Mach, throttle, fuel, oil pressure
- **Mini-map** — plots player, ally, and enemy positions from the live API, with map name display
- **Battle state** — health bar, velocity, compass rose, and fire-warning flash
- **HUD alerts** — scrollable list of the last 6 kills/damage/event messages with relative timestamps
- **Touch navigation** — bottom tab bar to switch between 4 screens
- **Wi-Fi setup portal** — first-boot captive portal (WiFiManager) for Wi-Fi credentials + PC IP entry
- **Persistent settings** — Wi-Fi SSID/password, PC IP, default screen, and brightness stored in NVS (no data loss on reboot)
- **Dual-core FreeRTOS** — HTTP polling on Core 0, UI rendering on Core 1, mutex-protected shared state
- **No-block UI** — display never freezes; "Waiting for War Thunder..." overlay shown when the game is not running

### Hardware

| Part | Notes |
|------|-------|
| [ESP32-2432S028 "CYD"](https://www.aliexpress.com/item/1005004502250619.html) | ~$10 on AliExpress |
| USB-C / Micro-USB cable | depends on your CYD revision |
| PC running War Thunder | same local network as the CYD |

### Installation

1. **Clone this repository**

   ```bash
   git clone https://github.com/BirdRa1n/WarThunderDeck.git
   cd WarThunderDeck
   ```

2. **Install PlatformIO**

   - [VS Code + PlatformIO IDE extension](https://platformio.org/install/ide?install=vscode), or
   - `pip install platformio` for the CLI.

3. **Build & upload**

   ```bash
   pio run --target upload
   ```

   Or click **Upload** in the PlatformIO toolbar inside VS Code.

4. **Monitor serial output** (optional)

   ```bash
   pio device monitor
   ```

### First-boot configuration

1. The display will show **"Start WiFi Setup"** on first boot.
2. Press the button — the device creates a Wi-Fi access point named **`WarThunderDeck`** (password: `warthunder`).
3. Connect your phone or laptop to that AP and visit **`http://192.168.4.1`**.
4. Select your home Wi-Fi network, enter the password, and fill in the **"War Thunder PC IP"** field with the LAN IP of your gaming PC (e.g. `192.168.1.50`).
5. Save — the device reconnects and shows the instruments screen automatically.

> **Finding your PC's IP:** on Windows run `ipconfig` in a terminal; look for the IPv4 address under your network adapter.

### War Thunder API used

| Endpoint | Purpose |
|----------|---------|
| `GET /indicators` | Vehicle instruments (speed, altitude, RPM, throttle, flaps, G-force, …) |
| `GET /state` | Battle state (health, heading, fire warning) |
| `GET /map_obj.json` | Object positions (player, allies, enemies) |
| `GET /map_info.json` | Map name and metadata |
| `GET /hudmsg` | HUD messages (kills, damage, events) |

> The War Thunder local API is active only while a battle is in progress. Make sure the game is running in a match.

### Roadmap

- [ ] Ground vehicles screen (tank-specific data: hull angle, engine RPM, ammo count)
- [ ] Naval vessels screen
- [ ] Mini-map background image from `/map.img` (requires PSRAM or progressive JPEG decode)
- [ ] OTA firmware update via web interface
- [ ] Brightness auto-adjustment via onboard LDR
- [ ] Multiple layout themes
- [ ] Multi-language support (EN / PT-BR)

### Contributing

1. Fork the repo.
2. Create a feature branch: `git checkout -b feat/my-feature`.
3. Commit your changes following [Conventional Commits](https://www.conventionalcommits.org/).
4. Open a Pull Request.

Please open an issue before starting significant work so we can discuss direction.

### Credits

- War Thunder local API documentation: [lucasvmx/WarThunder-localhost-documentation](https://github.com/lucasvmx/WarThunder-localhost-documentation)
- Display library: [Bodmer/TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
- Wi-Fi portal: [tzapu/WiFiManager](https://github.com/tzapu/WiFiManager)
- JSON parsing: [bblanchon/ArduinoJson](https://arduinojson.org/)

---

## Português

### O que é

O **WarThunderDeck** é um firmware de código aberto para o microcontrolador **ESP32-2432S028** (o popular "Cheap Yellow Display") que transforma o dispositivo em um **painel auxiliar de instrumentos para War Thunder**.

Ele se conecta à mesma rede Wi-Fi do seu PC e consulta a [API HTTP local do War Thunder](http://localhost:8111) para exibir telemetria em tempo real no display touchscreen de 2,8 polegadas.

### Funcionalidades

- **Instrumentos** — velocidade, altitude, bússola, RPM, fator G, Mach, acelerador, combustível
- **Minimapa** — posições de aliados e inimigos com grid e nome do mapa
- **Estado da partida** — barra de HP, alerta de incêndio (fundo pisca em vermelho)
- **Alertas HUD** — kills, danos e eventos com carimbo de tempo relativo
- **Configuração por portal Wi-Fi** — sem precisar recompilar: configure SSID e IP do PC direto pelo celular na primeira inicialização
- **Persistência** — configurações salvas na NVS (não perdidas ao reiniciar)

### Instalação

1. Clone o repositório e abra no **VS Code com PlatformIO**.
2. Conecte o CYD via USB e clique em **Upload**.
3. Na primeira inicialização, conecte-se à rede Wi-Fi **`WarThunderDeck`** (senha: `warthunder`) e acesse `192.168.4.1` para configurar.

### Licença

MIT — veja [LICENSE](LICENSE).
