Distributed3D
=============

A distributed 3d laser measurement system for 3D interior contour mapping.

Advantages over centralized solutions

* Compact, low cost subsystem is easier to replace and maintain should one fail;
* Easier to expand the overall range of measurement simply by adding more slave units;
* Less processing workload on the central system processor, raw data can be pre-processed locally from the slave unit then only send the key values to the central system.

Slave Unit
---

CS Defination | Assemly
---|---
<img src="./Model/Rendering/002.PNG" alt="drawing" height="400"/> | <img src="./Assembly.png" alt="Assembly" height="400"/>

An event-driven primitive soft RTOS is implemented in order to maintain responsiveness to the master's command while continiously measuring and tranmitting point cloud and miscellaneous data.

Each point measured in the <span style="color:red"> laser </span> CS is then represented in the <span style="color:blue"> ground </span> CS by the orthogonal following transformation,

<img src="https://latex.codecogs.com/svg.latex?\Large&space;p_1=T_{12} \cdot T_{23} \cdot p_3" title="\Large p_1=T_{12} \cdot T_{23} \cdot p_3" />

Master Panel
---

Configs the constants for the slave units and command the measurement strategy. A GUI enables the direct monitor of measurement process as well as real-time adjustment.

<img src="./GUI.png" alt="GUI" height="400"/>

Measurement Strategies
---

What makes a good strategy for a distributed measurement system?

None, in order to fully expliot the potential of a distributed measurement system, no strategy is good enough to be the one-hit wonder. It has to be as much versatile as possible and always ready to shift. This also help to solve the problem of potential overlapping or overlooking.

Here are a few show cases of how versatile the slave unit can be. Thanks to the singularity-free scanning motion and soft RTOS implemented, the measurement unit is able to

* Provide data as a series of point cloud in any specific range within the hemispherical workspace
* Respond to the master command in real-time to adjust measurement strategy.

Spiral | Vertical
---|---
![manual](./gif/spiral.gif) | ![manual](./gif/vertical.gif)

Manual | Partial
--- | ---
![manual](./gif/manual.gif) | ![vetical](./gif/partial.gif)

BOM
---

    n   Arduino DUE Board
    2*n AS5045 magnetic encoder
    2*n USR-WIFI232-T wifi module
    n   GP3D03-ZZ4-COM laser distance sensor
    n   2-DOF cloud deck
    2*n servo
    2*n Li-Po battery
    1   wireless router

    *n : number of units

Runtime environment
---
    NI LabVIEW 2013 runtime engine

License
---

    Distributed3D (both software and hardware drawings) is a free release under GPL v2.
