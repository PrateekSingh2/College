<div align="center">
  <h1>🏦 Vault OS</h1>
  
  <a href="https://crowcpp.org/"><img src="https://img.shields.io/badge/C%2B%2B17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++17"></a>
  <a href="https://crowcpp.org/"><img src="https://img.shields.io/badge/Crow-1A1A1A?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="Crow Framework"></a>
  <a href="https://tailwindcss.com/"><img src="https://img.shields.io/badge/Tailwind_CSS-38B2AC?style=for-the-badge&logo=tailwind-css&logoColor=white" alt="Tailwind CSS"></a>
</div>

<br>

<p align="center">
  Vault OS is a high-performance, full-stack core banking microservice. The backend is powered by modern C++17 using the Crow microframework and ASIO for asynchronous networking. The frontend is a sleek, dark-mode single-page application (SPA) built with HTML, JavaScript, and Tailwind CSS.
</p>

<hr>

## ✨ Key Features

<ul>
  <li><b>Account Management:</b> Create accounts, look up details, and view real-time balances.</li>
  <li><b>Secure Transactions:</b> Process deposits, withdrawals, and inter-account funds transfers safely.</li>
  <li><b>Mini Statements:</b> View the 5 most recent transactions with running balance history.</li>
  <li><b>Thread-Safe Backend:</b> Utilizes <code>std::mutex</code> to ensure safe concurrent API requests.</li>
  <li><b>SaaS-Style UI:</b> Responsive, glassmorphic frontend utilizing Tailwind CSS.</li>
</ul>

<hr>

## 🛠️ Complete Setup Guide (Windows)

<p>This project requires a modern 64-bit C++ environment to handle networking and threading properly.</p>

<details>
  <summary><b>1. Install MinGW-w64 from WinLibs</b> <i>(Click to expand)</i></summary>
  <blockquote>
    The standard MinGW is obsolete and 32-bit. We use WinLibs for an up-to-date 64-bit compiler.
  </blockquote>
  <ol>
    <li>Go to <a href="https://winlibs.com/">WinLibs.com</a>.</li>
    <li>Scroll down to the <b>Release versions</b> section.</li>
    <li>Download the <b>Zip archive</b> for <code>GCC (x.x.x) + MinGW-w64 - UCRT runtime</code>.</li>
    <li>Extract the downloaded zip file.</li>
    <li>Move the extracted <code>mingw64</code> folder directly into your <code>C:</code> drive. <i>(Path should be <code>C:\mingw64\bin\g++.exe</code>)</i>.</li>
  </ol>
</details>

<details>
  <summary><b>2. Configure Environment Variables</b> <i>(Click to expand)</i></summary>
  <ol>
    <li>Press the <b>Windows Key</b>, type <code>Environment Variables</code>, and hit Enter.</li>
    <li>Click the <b>Environment Variables...</b> button.</li>
    <li>In the bottom pane (System variables), find the variable named <b>Path</b>, select it, and click <b>Edit</b>.</li>
    <li>Click <b>New</b> and paste the exact path to the bin folder: <code>C:\mingw64\bin</code></li>
    <li><i>(Optional)</i> If you see any older MinGW paths (e.g., <code>C:\MinGW\bin</code>), select and delete them.</li>
    <li>Click <b>OK</b> on all windows. <b>Restart your terminal or VS Code</b>.</li>
  </ol>
</details>

<details>
  <summary><b>3. Install ASIO & Crow Dependencies</b> <i>(Click to expand)</i></summary>
  <ol>
    <li><b>ASIO:</b> Download the source zip from <a href="https://github.com/chriskohlhoff/asio/releases">ASIO Releases</a>. Extract it, and copy the <code>include</code> folder into your project directory.</li>
    <li><b>Crow:</b> Download <code>crow_all.h</code> from <a href="https://github.com/CrowCpp/Crow/releases">Crow Releases</a> and place it in your project root.</li>
  </ol>
</details>

### 📁 Project Folder Structure

Ensure your directory matches this exact layout before compiling:

```text
MicroProject/
│
├── include/                 # (Copied from ASIO download)
│   ├── asio.hpp
│   └── asio/
│
├── crow_all.h               # The Crow web framework header
├── main.cpp                 # The C++ backend API code
└── index.html               # The frontend dashboard UI
```

<hr>

🚀 Running the Project
Step 1: Compile the Backend
Open your terminal inside the MicroProject directory and run the following command.


<i><b>Note:</b> We explicitly link the Windows socket libraries (ws2_32, wsock32) and enforce the C++17 standard.</i>

Bash
```
g++ main.cpp -o backend -I ./include -lws2_32 -lwsock32 -std=c++17
```
(If g++ fails, use the absolute path: C:\mingw64\bin\g++.exe main.cpp ...)

Step 2: Start the Server
Run the generated executable to start listening for requests:

Bash
```
./backend.exe
```

<p>✅ You should see: <code>🚀 Core Banking API running on http://localhost:8080</code>. Keep this terminal open.</p>

Step 3: Launch the Frontend
Simply double-click the index.html file to open it in your web browser. No local frontend server is required; it fetches styling directly from the Tailwind CDN and communicates with your C++ backend locally.

<hr>

📡 API Endpoints Reference
<table>
<thead>
<tr>
<th>Method</th>
<th>Endpoint</th>
<th>Description</th>
<th>Payload Body</th>
</tr>
</thead>
<tbody>
<tr>
<td><kbd>POST</kbd></td>
<td><code>/api/account</code></td>
<td>Creates a new bank account</td>
<td><code>{"name": "string"}</code></td>
</tr>
<tr>
<td><kbd>GET</kbd></td>
<td><code>/api/account/&lt;id&gt;</code></td>
<td>Fetches account details/balance</td>
<td><i>None</i></td>
</tr>
<tr>
<td><kbd>GET</kbd></td>
<td><code>/api/account/&lt;id&gt;/statement</code></td>
<td>Fetches recent transactions</td>
<td><i>None</i></td>
</tr>
<tr>
<td><kbd>POST</kbd></td>
<td><code>/api/account/&lt;id&gt;/deposit</code></td>
<td>Deposits funds into account</td>
<td><code>{"amount": float}</code></td>
</tr>
<tr>
<td><kbd>POST</kbd></td>
<td><code>/api/account/&lt;id&gt;/withdraw</code></td>
<td>Withdraws funds from account</td>
<td><code>{"amount": float}</code></td>
</tr>
<tr>
<td><kbd>POST</kbd></td>
<td><code>/api/account/&lt;id&gt;/transfer</code></td>
<td>Transfers funds to another A/C</td>
<td><code>{"amount": float, "destination": int}</code></td>
</tr>
</tbody>
</table>

<hr>

<p align="center">
<i>Developed by <b>Prateek Singh</b> & <b>Shivansh Sahu</b></i>
</p>
