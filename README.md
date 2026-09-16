# ESP32 Smart Environmental Monitor (ESP-IDF & FreeRTOS)

Proyek ini adalah simulasi sistem monitoring lingkungan industri menggunakan **ESP-IDF (v5.x)** dan **FreeRTOS** pada mikrokontroler ESP32. Desain arsitektur ini menerapkan standar industri untuk aplikasi IoT yang andal, aman, dan efisien.

## 🚀 Fitur Utama & Konsep Industri
- **Multitasking Terjadwal (`FreeRTOS Tasks`)**: Memisahkan fungsi pembacaan sensor dan pemrosesan data ke dalam tugas-tugas independen yang berjalan secara paralel.
- **Inter-Task Communication (`FreeRTOS Queues`)**: Mengirimkan data struktur sensor antar-task secara aman tanpa risiko korupsi data memori.
- **Resource Locking (`FreeRTOS Mutex Semaphores`)**: Mengamankan output serial (UART/Stdout) agar log pencetakan data dari beberapa task tidak saling tumpang tindih.
- **Core Pinning (`Dual-Core Optimization`)**: Memanfaatkan arsitektur dual-core ESP32 secara efisien dengan menempatkan aplikasi utama pada Core 1, menjaga Core 0 tetap fokus pada fungsi sistem/jaringan.
- **Industrial Logging (`esp_log`)**: Menggunakan pustaka logging resmi untuk mempermudah pemantauan status sistem berdasarkan level prioritas (`INFO`, `WARN`, `ERROR`).

## 🛠️ Arsitektur Sistem
- **`Sensor_Read_Task`**: Berjalan setiap 2000ms dengan Prioritas 2 pada Core 1. Berfungsi mensimulasikan pembacaan sensor lingkungan dan mengirim hasilnya ke antrean data (*Queue*).
- **`Data_Processor_Task`**: Memiliki Prioritas 3 (lebih tinggi) pada Core 1. Menggunakan mode *blocking* (hemat daya) hingga ada data baru masuk ke antrean, lalu memeriksa ambang batas suhu (>30°C) untuk memicu sinyal peringatan (*Alert*).

## 📊 Hasil Build Sistem
- **Firmware Size**: ~160 KB (Sangat efisien dibandingkan framework Arduino).
- **Target Chip**: ESP32 Dual-Core.

## 💻 Cara Menjalankan Proyek
1. Clone repositori ini ke komputer Anda:
   ```bash
   git clone https://github.com
   ```
2. Buka folder proyek menggunakan VS Code yang sudah terinstal ekstensi **ESP-IDF**.
3. Hubungkan ESP32 ke komputer, lalu lakukan Build, Flash, dan Monitor:
   ```bash
   idf.py build flash monitor
   ```