---
sidebar_position: 1
title: Introduction
---

# Framework Introduction

This section will discuss the architecture design of the framework.
The application is primarily designed around the use of a "framework" architecture which is composed of several different parts.

These constituent parts include:

* Device Managers
* Hardware Abstraction Layer (HAL) Devices
* Messages/Events

![Architecture Diagram](./img/framework_arch_diagram.jpg)

Each of these different components will be discussed in detail in the following sections.

## Design Goals

The architectural design of the framework was centered around 3 primary goals:

1. Ease-of-use
2. Flexibility/Portability
3. Performance

In the course of a project's development, many problems can arise which hinder the speed of that development.
The framework architecture was designed to help combat those problems.

The framework is designed with the goal of speeding up the time to market for vision and other machine-learning applications.
In order to ensure a speedy time to market, it is critical that the software itself is easy to understand and easy to modify.
Keeping this goal in mind, the architecture of the framework was designed to be easy to modify without being restrictive, and without coming at the cost of performance.

## Relevant Files

The files which pertain to the framework architecture can primarily be found in the `framework/` folder of the specific application.
Because the application is designed around the use of the framework architecture, it is likely that the bulk of a developer's efforts will be focused on the contents of these folders.
