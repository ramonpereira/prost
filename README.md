# Prost

Prost is a probabilistic planning system that repeatedly computes which
action to take in the current state, executes that action by interacting
with [rddlsim](https://github.com/ssanner/rddlsim) and updates the
current state according to the outcome. The input language of Prost is
[RDDL](http://users.cecs.anu.edu.au/~ssanner/IPPC_2011/RDDL.pdf).

## Prerequisites

Make sure you meet the following requirements before installing Prost:

 * Linux operating system
 * git is installed to clone the repository
 * flex and bison are installed to parse the input
 * cmake and a c++ compiler (we recommend gcc) are installed to compile the planner
 * the BDD library [BuDDy](http://sourceforge.net/projects/buddy) is installed

On Linux, you can install all dependencies with the command

`sudo apt install git g++ cmake bison flex libbdd-dev`

If BuDDy is not found automatically, you can additionally set an
environment variable `$BDD_ROOT` which points to your BuDDY installation,
or you can adapt the file `src/cmake_modules/FindBDD.cmake`.

PROST executes the policy it computes by interacting with the rddlsim
server. We refer to the [github repository of
rddlsim](https://github.com/ssanner/rddlsim) for the latest installation
information. Without any guarantee for success, you can try the
following steps to install rddlsim:

 * `sudo apt install default-jre` (install Java runtime environment)
 * `git clone https://github.com/ssanner/rddlsim /path/to/rddlsim` (clone rddlsim repo)
 * `cd /path/to/rddlsim` (switch to directory)
 * `./compile` (compile rddlsim)

The Prost repo contains a script to run rddlsim
('testbed/run-server.py'). It requires that you set an environment
variable `$RDDLSIM_ROOT` that points to /path/to/rddlsim, e.g. by adding

'export RDDLSIM_ROOT=/path/to/rddlsim'

to the file ~/.bashrc. You don't have to use this script to run rddlsim,
but we assume in the following that you do.


## Installation

Installation of Prost is simple and can be done in three steps:

 * `git clone https://github.com/prost-planner/prost /path/to/prost` (clone Prost repo)
 * `cd /path/to/prost` (switch to directory)
 * `./build.py` (compile the planner in release mode)

If you need to compile the planner in debug mode, replace the last
command with `./build.py --debug`.

## Running Prost

Before you can run Prost, you have to start the rddlsim server by using
the provided script that can be found in the testbed folder. For
instance, running

`./run-server.py -b benchmarks/elevators-2011/`

from the testbed directory loads all instances of the elevators domain
of IPC 2011 into rddlsim (run `./run-server.py --help` for a full list
of available options).

Now you can start Prost with the `prost.py` script that can be found in
the root directory of the repo. For instance, to start planning for the
first elevators instance of IPC 2011 with the PROST version from IPC
2014, run

`./prost.py elevators_inst_mdp__1 "[Prost -s 1 -se [IPC2014]]"`

Note that there are many PROST configuration implemented. Run `prost.py`
without additional parameters for an overview of all available solvers
and their options.


## Performing experiments with Prost and Prost Lab

We recommend to perform experiments with the Prost planner by using
[Prost Lab](https://github.com/prost-planner/prostlab), an experiment
package that is based on the [Lab
toolkit](https://github.com/aibasel/lab). Information on how to set up
Prost Lab can be found
[here](https://github.com/prost-planner/prost/wiki/Evaluation).

If you want to compare your planner to Prost, please make sure to
compile Prost in release mode and to use an appopriate search engine for
Prost. If in doubt, run both the Prost-2011 and Prost-2014 planners by
using `-se [IPC2011]` and `-se [IPC2014]` in the call to `prost.py`. If
you don't have the time to use both configurations, we recommend to go
with Prost-2014.

Please cite the Prost planner as well as the Prost-2011 version of the
planner with

```
@InProceedings{keller-eyerich-icaps2012,
  author =       "Thomas Keller and Patrick Eyerich",
  title =        "{PROST}: Probabilistic Planning Based on {UCT}",
  booktitle =    "Proceedings of the Twenty-Second International Conference on
                  Automated Planning and Scheduling (ICAPS 2012)",
  year =         "2012",
  publisher =    "AAAI Press"
  pages =        "119--127"
}
```

and the Trial-based Heuristic Tree Search framework or the Prost-2014
version of Prost with either

```
@InProceedings{keller-helmert-icaps2013,
  author =       "Thomas Keller and Malte Helmert",
  title =        "Trial-based Heuristic Tree Search for Finite Horizon {MDPs}",
  booktitle =    "Proceedings of the Twenty-Third International Conference on
                  Automated Planning and Scheduling (ICAPS 2013)",
  year =         "2013",
  publisher =    "AAAI Press"
  pages =        "135--143"
}
```

or

```
@PhdThesis{keller-phd2015,
  author =       "Thomas Keller",
  title =        "Anytime Optimal {MDP} Planning with Trial-based Heuristic Tree Search",
  school =       "University of Freiburg",
  year =         "2015"
}
```

## Contributors

If you have any questions, feel free to contact one of the currently
actively core developers:

 * [Thomas Keller](mailto:tho.keller@unibas.ch?subject=[Prost])
 * [Florian Geißer](mailto:florian.geisser@anu.edu.au?subject=[Prost])
 * [David Speck](mailto:speckd@informatik.uni-freiburg.de?subject=[Prost)

There are quite a few additional people who have contributed to PROST
since the planner's development was started around December 2010 (these
are ordered alphabetically):

 * Augusto Blaas Correa
 * Dorde Relic
 * Florent Teichteil-Koenigsbuch
 * Jendrik Seipp
 * Joris Scharpff
 * Max Grüner
 * Patrick Eyerich
 * Scott Sanner

In addition to these, the developers of [Fast
Downward](http://fast-downward.org) deserve to be mentioned. Many
decisions have been based on discussion with the Fast Downward
developers, and there are even some code fragments that have been
borrowed from the Fast Downward code.

