# Rules

This directory contains the rules for the RESTART project. There are two types of rules:

* **Reactive rules**: These rules are triggered by events.
* **Deliberative rules**: These rules are used to plan activities.

## Reactive Rules

The follwoing rules are defined:

* **Awake**: Checks if the user is awake. If the user is lying down, the rule will trigger an alert.
* **Remind**: Reminds something to the user. If the user does not respond, the rule will trigger an alert.
* **PhysicalStimulation**: Stimulates the user to perform physical activity.
* **CognitiveStimulation**: Stimulates the user to perform cognitive activity.

The following diagram describes the **Awake** rule:

```mermaid
stateDiagram-v2
    direction LR
    [*] --> CheckPose
    CheckPose --> [*]: Standing/Sitting
    CheckPose --> Alert: Lying
    Alert --> [*]
```

The following diagram describes the **Remind** rule:

```mermaid
stateDiagram-v2
    direction LR
    [*] --> Check(topic)
    Check(topic) --> [*]: Done
    Check(topic) --> Remind(topic): Not done
    Remind(topic) --> [*]: Ok
    Remind(topic) --> Alert: No
    Alert --> [*]
```

The following diagram describes the **PhysicalStimulation** rule:

```mermaid
stateDiagram-v2
    direction LR
    [*] --> CheckHealthState
    CheckHealthState --> SuggestPhysicalActivity(activity): Ok
    CheckHealthState --> Alert: Not ok
    SuggestPhysicalActivity(activity) --> PerformActivity(activity): Ok
    SuggestPhysicalActivity(activity) --> Alert: No
    PerformActivity(activity) --> [*]: Done
    PerformActivity(activity) --> Alert: Failure
    FindUser --> Alert: Not found
    Alert --> [*]
```

The following diagram describes the **CognitiveStimulation** rule:

```mermaid
stateDiagram-v2
    direction LR
    [*] --> CheckHealthState
    CheckHealthState --> SuggestCognitiveActivity(activity): Ok
    CheckHealthState --> Alert: Not ok
    SuggestCognitiveActivity(activity) --> PerformAudioActivity(activity): Audio
    SuggestCognitiveActivity(activity) --> PerformVideoActivity(activity): Video
    SuggestCognitiveActivity(activity) --> PerformGameActivity(activity): Game
    SuggestCognitiveActivity(activity) --> Alert: No
    PerformAudioActivity(activity) --> [*]: Done
    PerformVideoActivity(activity) --> [*]: Done
    PerformGameActivity(activity) --> [*]: Done
    PerformActivity(activity) --> Alert: Failure
    FindUser --> Alert: Not found
    Alert --> [*]
```

## Deliberative Rules

The following rules are defined:

* **FindUser**: Finds the user.
* **PhysicalActivity**: Plans a physical activity.
* **CognitiveActivity**: Plans a cognitive activity.
* **WellBeing**: Plans a well-being activity.

The following diagram describes the **FindUser** rule:

```mermaid
stateDiagram-v2
    direction LR
    FindUser
    Note right of FindUser: duration <= 1m
```

The following diagram describes the **PhysicalActivity** rule:

```mermaid
stateDiagram-v2
    direction LR
    FindUser --> PhysicalActivity
    Note right of PhysicalActivity: duration <= 5m<br/>fu.end == start
```

The following diagram describes the **CognitiveActivity** rule:

```mermaid
stateDiagram-v2
    direction LR
    FindUser --> CognitiveActivity
    Note right of CognitiveActivity: duration <= 5m<br/>fu.end == start
```

The following diagram describes the **WellBeing** rule:

```mermaid
stateDiagram-v2
    direction LR
    CognitiveActivity --> WellBeing
    PhysicalActivity --> WellBeing
    note right of WellBeing
        ca.start >= 10:00 #8743; ca.end <= 12:00
        pa.start >= 12:00 #8743; pa.end <= 17:00
    end note
```
