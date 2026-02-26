<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>MrHouseMonitor</title>
<style>
    body {
        font-family: "Segoe UI", Tahoma, Geneva, Verdana, sans-serif;
        background-color: #f8f9fa;
        color: #212529;
        line-height: 1.6;
        margin: 0;
        padding: 0 20px;
    }
    header {
        background-color: #343a40;
        color: #fff;
        padding: 20px;
        text-align: center;
        border-bottom: 4px solid #17a2b8;
    }
    header h1 {
        margin: 0;
        font-size: 2em;
    }
    section {
        margin: 30px 0;
        padding: 20px;
        background: #fff;
        border-radius: 12px;
        box-shadow: 0 3px 6px rgba(0,0,0,0.1);
    }
    table {
        width: 100%;
        border-collapse: collapse;
        margin-top: 15px;
    }
    table, th, td {
        border: 1px solid #dee2e6;
    }
    th, td {
        padding: 12px;
        text-align: left;
    }
    th {
        background-color: #17a2b8;
        color: #fff;
    }
    h2 {
        color: #17a2b8;
    }
    .note {
        background-color: #e2f0fb;
        padding: 10px;
        border-left: 4px solid #17a2b8;
        margin-top: 15px;
        border-radius: 5px;
    }
    code {
        background-color: #f1f3f5;
        padding: 2px 5px;
        border-radius: 4px;
        font-family: monospace;
    }
    .quickstart pre {
        background-color: #f1f3f5;
        padding: 15px;
        border-radius: 8px;
        overflow-x: auto;
    }
    footer {
        text-align: center;
        margin: 50px 0 20px;
        color: #6c757d;
    }
</style>
</head>
<body>

<header>
    <h1>🏠 MrHouseMonitor</h1>
    <p>Mr House on your 2.4" CYD display — a sleek ESP32 smart display interface</p>
</header>

<section>
    <h2>⚡ Features</h2>
    <ul>
        <li>Real-time display on 2.4" TFT</li>
        <li>Customizable GUI using <code>LVGL</code></li>
        <li>Optimized graphics via <code>TFT_eSPI</code></li>
        <li>Easy integration with images and Python scripts</li>
    </ul>
</section>

<section>
    <h2>📂 Important Files</h2>
    <table>
        <tr>
            <th>File</th>
            <th>Location</th>
            <th>Purpose</th>
        </tr>
        <tr>
            <td><code>lv_conf.h</code></td>
            <td><code>~/Arduino/libraries</code></td>
            <td>LVGL configuration (GUI features, memory, behavior)</td>
        </tr>
        <tr>
            <td><code>User_Setup.h</code></td>
            <td><code>~/Arduino/libraries/TFT_eSPI</code></td>
            <td>Display & SPI settings for TFT_eSPI</td>
        </tr>
        <tr>
            <td>Image files</td>
            <td>Same folder as <code>.ino</code></td>
            <td>Assets displayed on the screen</td>
        </tr>
    </table>
    <div class="note">⚠️ Make sure to keep files in these exact locations to avoid errors.</div>
</section>

<section>
    <h2>🛠 Setup Instructions</h2>
    <ol>
        <li><strong>Install Required Libraries</strong>
            <ul>
                <li>LVGL</li>
                <li>TFT_eSPI</li>
            </ul>
        </li>
        <li><strong>Configure Your Board</strong>
            <ul>
                <li>Select <code>ESP32 Dev Module</code> in Arduino IDE</li>
            </ul>
        </li>
        <li><strong>Place Files Correctly</strong>
            <ul>
                <li><code>lv_conf.h</code> → <code>~/Arduino/libraries</code></li>
                <li><code>User_Setup.h</code> → <code>~/Arduino/libraries/TFT_eSPI</code></li>
                <li>Images → same folder as your <code>.ino</code> file</li>
            </ul>
        </li>
        <li><strong>Python Port Setup</strong>
            <ul>
                <li>Ensure the serial port in the Python script matches your ESP32 connection</li>
            </ul>
        </li>
    </ol>
</section>

<section>
    <h2>💡 Notes</h2>
    <ul>
        <li><strong>LVGL (<code>lv_conf.h</code>)</strong> → Controls GUI features, memory allocation, and display behavior.</li>
        <li><strong>TFT_eSPI (<code>User_Setup.h</code>)</strong> → Configures SPI pins and display type for fast rendering.</li>
        <li><strong>Images</strong> → Store in <code>.ino</code> folder for easy loading.</li>
        <li>Keep proper file structure to avoid compilation/runtime issues.</li>
    </ul>
</section>

<section class="quickstart">
    <h2>🚀 Quick Start</h2>
    <pre>
1. Connect ESP32 to your PC
2. Open the <code>.ino</code> file in Arduino IDE
3. Verify libraries and board settings
4. Upload to ESP32 and enjoy Mr House on your 2.4" CYD display
    </pre>
</section>

<footer>
    📜 MIT License – free to use, modify, and share
</footer>

</body>
</html>
