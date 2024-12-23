FROM ubuntu:14.04

RUN sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
RUN apt install curl -y
RUN curl -s https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | sudo apt-key add -
RUN apt update && apt install dpkg
RUN apt install ros-indigo-ros-base ros-indigo-catkin python-catkin-tools -y

RUN rosdep init
RUN rosdep update

SHELL ["/bin/bash", "-c"]
RUN echo "source /opt/ros/indigo/setup.bash" >> ~/.bashrc && source ~/.bashrc

CMD /bin/bash