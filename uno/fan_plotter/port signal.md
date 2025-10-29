How to make your own serial plotter
In this tutorial we'll use a couple of my favorite tools and tricks for quickly making our own serial plotter that lives in our browser. I'll be writing things in javascript/web, so some basic knowledge about how a browser works and html/javascript is useful.

Prerequisites
You'll need npm to install packages. This is not strictly necassary; you can pull in stuff using CDN; but I think its nice to have a bit more automated tooling. For this tutorial we'll be using Vite to build, host, pack things together.

First thing to do is to install npm. I encourage you to watch this Coding Train tutorial if you've never used node and this for learning a bit more about the npm packet manager.

Setting things up
Once you have node install we'll set up a Vite project. I'll use the osx terminal to navigate. It should be fairly similar on Windows.

mkdir smoothie_app && cd smoothie_app
 
#then we'll init a npm project and setup vite
npm create vite@latest ./
This will give you some prompts. For now we'll choose vanilla style and plain javascript. Choose default answers on the following prompts. This should spin up a webserver on localhost with your app. Go to it and check if you see "Hello Vite".

What just happened: we made a folder and used npm to set up Vite inside it. Vite helps us pull in JavaScript packages from the web and put them together into one project so the browser can run them. It also gives us a simple way to start a local website on our computer, so we can see changes immediately while we work. Later, the same setup can prepare everything into files ready to be shared or put online.

Moving on. I recommend that you know exit the running Vite server in terminal and open the folder we just made in an IDE. I'm using VScode. Once opened you should see the following file structure:

smoothie_app/
├── index.html
├── package.json
├── package-lock.json
├── node_modules/
├── vite.config.js
└── src/
    ├── main.js
    └── style.css
Open the index.html file and main.js file and familiarize yourself with it. This is where we will add code for visualizing data, but first let's install smoothieChart. Open up terminal again and run npm install smoothie . This adds smoothiecharts to our app. To double-check, see if smoothie was added to the package.json file in your project. Ok? Ok. Let's test it. Open index.html and a canvas we can draw on:

<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <link rel="icon" type="image/svg+xml" href="/vite.svg" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>smoothie_hello</title>
  </head>
  <body>
    <div id="app">
      <canvas id="chart" width="800" height="200"></canvas>
    </div>
    <script type="module" src="/src/main.js"></script>
  </body>
</html>
Now open the main.js script, and replace the old code with:

how doimport { SmoothieChart, TimeSeries } from "smoothie";
 
const canvas = document.getElementById("chart");
 
// Create chart
const chart = new SmoothieChart({
  millisPerPixel: 20,
  interpolation: "linear",
  grid: { millisPerLine: 1000, verticalSections: 4 },
  tooltip: true,
});
 
// Stream to canvas
chart.streamTo(canvas, /* delay ms */ 500);
 
// Create a series and add it
const series = new TimeSeries();
chart.addTimeSeries(series, { lineWidth: 2 });
 
// Demo data
setInterval(() => {
  series.append(Date.now(), Math.sin(Date.now() / 500) * 50 + 50);
}, 50);
This adds a smoothie chart to canvas element with a "stream" of random data in it. To run the app type npm run dev in terminal on root in the project.

Now, let's work on bringing in WebSerial. This is kind of messy and I encourage you to read up on documentation here.

Here's the complete main.js:

import { SmoothieChart, TimeSeries } from "smoothie";
 
const btn = document.getElementById("connect");
 
console.log(btn);
 
let latestValues = [0, 0, 0]; // global
 
btn.addEventListener("click", async () => {
  const port = await navigator.serial.requestPort();
  await port.open({ baudRate: 115200 });
 
  const decoder = new TextDecoder();
  let buf = "";
 
  while (port.readable) {
    const reader = port.readable.getReader();
    try {
      while (true) {
        const { value, done } = await reader.read();
        if (done) break;
        if (!value) continue;
 
        // accumulate and split on newline
        buf += decoder.decode(value, { stream: true });
        const lines = buf.split(/\r?\n/);
        buf = lines.pop() ?? "";
 
        for (const line of lines) {
          if (!line) continue;
          // show latest complete line
          // display.textContent = line;
 
          const parts = line.split(",");
 
          latestValues = parts;
          console.log(latestValues); // ["-38.18","2.73","-76.57"]
        }
      }
    } catch (err) {
      // display.textContent = "Error: " + err.message;
    } finally {
      // flush any remaining bytes to buffer (optional)
      const tail = decoder.decode();
      if (tail) buf += tail;
      reader.releaseLock();
    }
  }
});
 
function makeChart(canvasId, color) {
  const chart = new SmoothieChart({
    millisPerPixel: 20,
    interpolation: "linear",
    grid: { millisPerLine: 1000, verticalSections: 4 },
    tooltip: true,
    minValue: -150, // set lower bound
    maxValue: 150, // set upper bound
  });
  const canvas = document.getElementById(canvasId);
  chart.streamTo(canvas, 500);
  const series = new TimeSeries();
  chart.addTimeSeries(series, { lineWidth: 2, strokeStyle: color });
  return series;
}
 
const sX = makeChart("chartX", "red");
const sY = makeChart("chartY", "green");
const sZ = makeChart("chartZ", "blue");
 
setInterval(() => {
  const now = Date.now();
  const [x, y, z] = latestValues;
  sX.append(now, x);
  sY.append(now, y);
  sZ.append(now, z);
}, 10);
Remember to add elements in html.