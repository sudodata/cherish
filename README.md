# Dura-EUropos In Situ - (DÉdUIS)\* 
The InSitu library software is a heterogeneous framework for the modeling and representation of historical structures and archaeological sites in 3D. It is designed so that to unify the currently disparate data types available (e.g. 2D images, 3D strokes entered by user, text annotations attached to 3D locations, mesh representations of buildings) and to use the strength of each to support the others. For the data processing, rather than relying on either purely automatic approaches or human input, the framework supports speculation and reasoning in the form of both. As a result, the software is capable to take as input various raw heterogeneous data source and produce a labeled, corresponded and augmented data source that can be used in variety of applications.

\* *déduis* - (I) deduce, from French

###### Development status
Currently converting the prototype version into alpha version, very active development. The current state of the project is not usable yet. Planned steps: 
* Version control on github
* Functional / technical specification
Reduce the library dependencies by using only Qt, OpenGL and cmake
* Improve GUI
* cmake (or qmake) files, installation procedure steps (what technical expertise is required, tutorial-style examples)
* Better object orientation design, refactoring and introducing new features
* Unit testing, GUI testing
* Documentation for user (monolithic) and wiki (software set up, overview of how it works, guides to doing common tasks)
* Bug database, feature requests 
* History of past releases 
* Potential developer guidelines (where documentation is incomplete, desired features, known deficiencies, etc.)
* FAQ section or file
* Demos, screen shots, videos, example outputs

![Alt text](https://github.com/vicrucann/dura-europos-insitu/blob/master/image-docs/gui-orig.png "Prototype GUI screenshot")  
The initial prototype screen shot.

## Content
[Motivation](https://github.com/vicrucann/dura-europos-insitu/#motivation)  
[Features](https://github.com/vicrucann/dura-europos-insitu/#features)  
[Requirements](https://github.com/vicrucann/dura-europos-insitu/#requirements)  
[Installation](https://github.com/vicrucann/dura-europos-insitu/#installation)  
[Quick start](https://github.com/vicrucann/dura-europos-insitu/#quick-start)  
[Program usage wiki](https://github.com/vicrucann/dura-europos-insitu/#program-usage-wiki)  
[Development and contribution](https://github.com/vicrucann/dura-europos-insitu/#development-and-contribution)  
[License](https://github.com/vicrucann/dura-europos-insitu/#license)  
[Contact and authors info](https://github.com/vicrucann/dura-europos-insitu/#contact-and-authors-info)  
[References](https://github.com/vicrucann/dura-europos-insitu/#references)  

## Motivation

Modeling historical sites is important in many fields, including cultural heritage preservation, architectural design, urban planning, etc. For any given site, there may exist many different representations such as photos, drawings, maps, laser scans, 3D renderings. Yet it remains challenging to exploit the full range of the data types, especially when given data source is sparse and incomplete.

The presented InSitu software creates a heterogeneous 3D representation of archaeological sites that improve our understanding of the past while supporting novel applications in the present. It is capable of annotation the collected site and artifacts with evidence (e.g., geo-location, scan data accuracy) and confidence estimation (e.g., for text annotations), and use this information to inform the reconstruction process. In the future, it is possible to expand the types of applications for which such data can be used.

## Features

* (Automated, planned) **3D modeling**: image-based modeling, laser scan data, procedural models.
* **Markup system**, based on 3D sketching (interpretation of 2D sketches as 3D forms): intuitive and interactive annotations, labeling and correspondence.
* **Human input** (crowd sourcing): augmentation, labeling and alignments of the sparse, multi-scale and heterogeneous data sources. 
* (planned) **Machine learning**: improvements of digital computation algorithms and reductions of the dependency on human computation. E.g., usage of semi-supervised learning and active learning to reduce uncertainty in the data.

## Requirements

## Installation

## Quick start

## Program usage wiki

## Development and contribution 

## License

The InSitu is an open source software and is under GNU GPL-v3 license. See the corresponding [LICENSE](https://github.com/vicrucann/dura-europos-insitu//blob/master/LICENSE) file for details.

## Contact and authors info

## References

Here represented the publications that led to the InSitu software:  

1. An integrated image and sketching environment for archaeological sites - Xuejin Chen, Yann Morvan, Yu He, Julie Dorsey, Holly Rushmeier  
2. The Mental Canvas: a tool for conceptual architectural design and analysis - Julie Dorsey, Songhua Xu, Gabe Smedresman, Holly Rushmeier, Leonard McMillan
