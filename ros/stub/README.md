## Topic

This topic is used to send a command to the robot.

`/nao_command`

## Message Definition

The message type is `pepper_speech/ActionMode`.

The message is composed by:

```c
string command
string modality
```

This message is used to send a command to the robot. The `command` is a string that can be one of the following values:
- `"1"`, `greetings`, Introducing the patient to the CT
- `"2"`, `instructing`, Explaining the CT tasks to the patient
- `"3"`, `suggesting`, Providing suggestions to better perform a particular task of the CT
- `"4"`, `feedbacking`, Providing feedback on performance
- `"5"`, `closing`, Dismiss the patient because the CT ended

The `modality` is a string that can be one of the following values:
- `"1"`, `formal`, Formal and polite
- `"2"`, `informal`, Informal

## Adding a message type

To add a new message type, you need to edit the CMakeLists.txt file and the package.xml file.

Add the following line to the CMakeLists.txt file:

```cmake
find_package(catkin REQUIRED COMPONENTS
  message_generation
)
```

You also need to add the message type to the list of messages in the CMakeLists.txt file:

```cmake
add_message_files(
  FILES
  ActionMode.msg
)
```

Finally, you need to add the message type to the package.xml file:

```xml
<build_depend>message_generation</build_depend>
<exec_depend>message_runtime</exec_depend>
```

## Usage

Sending a command to the robot:

```bash
rostopic pub /nao_command pepper_speech/ActionMode "{command: '1', modality: '1'}"
```

The robot will publish a completion message on the topic `/command_completed` when the command is completed, with the message type `std_msgs/String` containing the id of the command that was completed.

```c
string id
```

For example, if the command `1` was completed, the robot will publish the following message:

```bash
rostopic pub /command_completed std_msgs/String "1"
```