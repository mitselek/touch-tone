# Phone Zero MVP Component List

## Overview

This document lists all components selected for the Phone Zero MVP, with priority given to development boards and off-the-shelf modules to accelerate development time.

## Core Processing & Communication

<table>
  <thead>
    <tr>
      <th align="left">Component</th>
      <th align="left">Selected Part</th>
      <th align="left">Key Specifications</th>
      <th align="left">Est. Cost</th>
      <th align="left">Supplier</th>
      <th align="left">Rationale</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Combined ESP32 + 4G Module</td>
      <td>LILYGO T-SIM7600</td>
      <td>
        <ul>
          <li>ESP32-WROVER-E</li>
          <li>SIM7600 4G module</li>
          <li>PCB antenna</li>
          <li>MicroSIM slot</li>
        </ul>
      </td>
      <td>$45.99</td>
      <td>Amazon</td>
      <td>All-in-one solution eliminates integration challenges</td>
    </tr>
    <tr>
      <td>Alternative ESP32 + 4G</td>
      <td>Waveshare SIM7600E-H 4G</td>
      <td>
        <ul>
          <li>4G LTE Cat-4</li>
          <li>Global frequency bands</li>
          <li>USB interface</li>
          <li>AT command control</li>
        </ul>
      </td>
      <td>$59.99</td>
      <td>Waveshare</td>
      <td>Extensive documentation and support available</td>
    </tr>
  </tbody>
</table>

## Audio System

<table>
  <thead>
    <tr>
      <th align="left">Component</th>
      <th align="left">Selected Part</th>
      <th align="left">Key Specifications</th>
      <th align="left">Est. Cost</th>
      <th align="left">Supplier</th>
      <th align="left">Rationale</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Speaker</td>
      <td>Adafruit Mono Enclosed Speaker</td>
      <td>
        <ul>
          <li>3W</li>
          <li>4Ω</li>
          <li>40mm diameter</li>
        </ul>
      </td>
      <td>$7.50</td>
      <td>Adafruit</td>
      <td>Pre-enclosed, easy to mount, good volume</td>
    </tr>
    <tr>
      <td>Speaker Driver</td>
      <td>Adafruit MAX98357A</td>
      <td>
        <ul>
          <li>I2S input</li>
          <li>3.2W output</li>
          <li>Class D amplifier</li>
        </ul>
      </td>
      <td>$5.95</td>
      <td>Adafruit</td>
      <td>Direct I2S from ESP32, simple integration</td>
    </tr>
    <tr>
      <td>Microphone</td>
      <td>Adafruit MEMS Microphone Breakout</td>
      <td>
        <ul>
          <li>Omnidirectional</li>
          <li>-26dB sensitivity</li>
          <li>I2S digital output</li>
        </ul>
      </td>
      <td>$4.95</td>
      <td>Adafruit</td>
      <td>Digital output reduces noise issues</td>
    </tr>
  </tbody>
</table>

## User Interface (for cost estimation)

<table>
  <thead>
    <tr>
      <th align="left">Component</th>
      <th align="left">Selected Part</th>
      <th align="left">Key Specifications</th>
      <th align="left">Est. Cost</th>
      <th align="left">Supplier</th>
      <th align="left">Rationale</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Large Tactile Buttons</td>
      <td>Adafruit 16mm Illuminated Pushbuttons</td>
      <td>
        <ul>
          <li>16mm diameter</li>
          <li>Panel mountable</li>
          <li>Momentary action</li>
        </ul>
      </td>
      <td>$2.95 x 6</td>
      <td>Adafruit</td>
      <td>Large enough for easy location, distinct tactile feel</td>
    </tr>
    <tr>
      <td>Button Caps</td>
      <td>Game console buttons</td>
      <td>
        <ul>
          <li>Various textures</li>
          <li>Large size</li>
          <li>Snap-on design</li>
        </ul>
      </td>
      <td>$14.95</td>
      <td>Amazon</td>
      <td>Different shapes provide tactile differentiation</td>
    </tr>
  </tbody>
</table>

## Power System

<table>
  <thead>
    <tr>
      <th align="left">Component</th>
      <th align="left">Selected Part</th>
      <th align="left">Key Specifications</th>
      <th align="left">Est. Cost</th>
      <th align="left">Supplier</th>
      <th align="left">Rationale</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Battery</td>
      <td>Adafruit Lithium Ion Battery</td>
      <td>
        <ul>
          <li>3.7V</li>
          <li>2500mAh</li>
          <li>JST connector</li>
        </ul>
      </td>
      <td>$14.95</td>
      <td>Adafruit</td>
      <td>Adequate capacity for MVP testing</td>
    </tr>
    <tr>
      <td>Battery Management</td>
      <td>Adafruit PowerBoost 1000C</td>
      <td>
        <ul>
          <li>1000mA charge rate</li>
          <li>5V output</li>
          <li>USB-C input</li>
        </ul>
      </td>
      <td>$19.95</td>
      <td>Adafruit</td>
      <td>Handles charging and provides regulated power</td>
    </tr>
    <tr>
      <td>Power Supply</td>
      <td>5V 2A USB-C Power Supply</td>
      <td>
        <ul>
          <li>5V DC</li>
          <li>2A output</li>
          <li>USB-C connector</li>
        </ul>
      </td>
      <td>$7.95</td>
      <td>Various</td>
      <td>Standard power supply, widely available</td>
    </tr>
  </tbody>
</table>

## Enclosure & Assembly

<table>
  <thead>
    <tr>
      <th align="left">Component</th>
      <th align="left">Selected Part</th>
      <th align="left">Key Specifications</th>
      <th align="left">Est. Cost</th>
      <th align="left">Supplier</th>
      <th align="left">Rationale</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Enclosure</td>
      <td>Hammond 1591ESBK</td>
      <td>
        <ul>
          <li>191 x 110 x 61 mm</li>
          <li>ABS plastic</li>
          <li>Black</li>
        </ul>
      </td>
      <td>$12.95</td>
      <td>Mouser</td>
      <td>Sufficient space, easy to modify for buttons</td>
    </tr>
    <tr>
      <td>Prototype Board</td>
      <td>Perma-Proto Half-sized</td>
      <td>
        <ul>
          <li>3.3 x 2.1"</li>
          <li>Mounting holes</li>
          <li>Breadboard layout</li>
        </ul>
      </td>
      <td>$4.95</td>
      <td>Adafruit</td>
      <td>Allows for semi-permanent assembly</td>
    </tr>
    <tr>
      <td>Wiring</td>
      <td>Silicone-cover stranded wire</td>
      <td>
        <ul>
          <li>26AWG</li>
          <li>Multiple colors</li>
          <li>Flexible</li>
        </ul>
      </td>
      <td>$16.95</td>
      <td>Amazon</td>
      <td>Flexible, easy to work with for prototype</td>
    </tr>
  </tbody>
</table>

## Estimated Total Cost: $~220

## Notes

- Components selected prioritize ease of integration and availability over optimization
- Development board approach eliminates need for custom PCB design
- Selected parts have established libraries and sample code where possible
- Cost estimates do not include shipping or taxes
