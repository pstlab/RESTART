# Rules

```mermaid
stateDiagram-v2
    direction LR
    [*] --> ShowFigs(1)
    ShowFigs(1) --> Say(intro): 10s
    Say(intro) --> Note(1): correct
    Say(intro) --> Repeat(intro): 8s
    Say(intro) --> Say(intro): asks
    Repeat(intro) --> Failure: 8s
    Failure --> [*]
    Repeat(intro) --> Note(1): correct
    Repeat(intro) --> Failure: wrong
    Note(1) --> Say(intro2)
    state fork_state <<fork>>
    Say(intro2) --> fork_state
    fork_state --> ShowFigs(n)
    fork_state --> DescribeFigs(n)
    DescribeFigs(n) --> Note(R,n): 8s
    Note(R,n) --> RepeatFigs(n)
    RepeatFigs(n) --> Note(N,n): 8s
    RepeatFigs(n) --> Note(n): correct
    RepeatFigs(n) --> Note(S,n): wrong
    Note(N,n) --> Next(n)
    state join_state <<join>>
    Next(n) --> join_state
    ShowFigs(n) --> join_state
    join_state --> fork_state: n < 18
    join_state --> Success: n = 18
    Success --> [*]
    DescribeFigs(n) --> Note(S,n): wrong
    Note(S,n) --> Note(N,n): 8s
    Note(S,n) --> Note(S,n): wrong
    Note(S,n) --> Note(n): correct
    Note(n) --> Next(n): 5s
    Note(n) --> Note(S,n): another
    DescribeFigs(n) --> Note(N,n): talks
    DescribeFigs(n) --> Say(other): persists
    Say(other) --> Note(S,n)
    DescribeFigs(n) --> RepeatFigs(n): distract
```

This directory contains the rules for the RESTART project. There are two types of rules:

* **Deliberative rules**: These rules are used to plan activities.
* **Reactive rules**: These rules are triggered by events.

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
        pa.start >= 15:00 #8743; pa.end <= 17:00
    end note
```

The following diagram describes a possible plan for achieving a **WellBeing** goal:

```mermaid
gantt
    title A well being plan
    dateFormat HH:mm
    axisFormat %H:%M
    section Plan
        FindUser          :a1, 10:59, 1m
        CognitiveActivity :a2, after a1, 3m
        FindUser          :a3, 14:59, 1m
        PhysicalActivity  :a4, after a3, 4m
```

## Reactive Rules

The follwoing rules are defined:

* **FindUser**: Finds the user.
* **Awake**: Checks if the user is awake. If the user is lying down, the rule will trigger an alert.
* **Remind**: Reminds something to the user. If the user does not respond, the rule will trigger an alert.
* **PhysicalStimulation**: Stimulates the user to perform physical activity.
* **CognitiveStimulation**: Stimulates the user to perform cognitive activity.

The following diagram describes the **FindUser** rule:

```mermaid
stateDiagram-v2
    direction LR
    [*] --> FindUser
    FindUser --> [*]: Found
    FindUser --> Alert: Not found
    Alert --> [*]
```

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

## Low-level commands

The following low-level commands are defined:

* **FindUser**: Finds the user. The robot navigates inside the house until the user is found.
* **CheckPose**: Checks the user's pose. Through ... **what?**, the robot checks if the user is standing or sitting. If the user is lying down, the robot triggers an alert.
* **Check(topic)**: Checks if the user has done something. Through verbal interaction, the robot checks if the user has done something.
* **Remind(topic)**: Reminds something to the user. Through verbal interaction, the robot reminds something to the user.
* **SuggestPhysicalActivity(activity)**: Suggests a physical activity to the user. Through verbal interaction, the robot suggests a physical activity to the user.
* **SuggestCognitiveActivity(activity)**: Suggests a cognitive activity to the user. Through verbal interaction, the robot suggests a cognitive activity to the user.
* **PerformAudioActivity(activity)**: Performs an audio activity. The robot plays an audio.
* **PerformVideoActivity(activity)**: Performs a video activity. The robot plays a video.
* **PerformGameActivity(activity)**: Performs a game activity. The robot starts a game.
