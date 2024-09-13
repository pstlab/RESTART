# RESTART (Robot Enhanced Social abilities based on Theory of mind for Acceptance of Robot in assistive Treatments)

The RESTART project aims at designing and developing a new human-robot interactive paradigm, in which the robot is humanized thanks to empowered AI-based social abilities based on the concept of theory of mind and mutual-understanding in order to improve the acceptability of robots especially in assistance scenarios, where higher legibility and conformity to cognitive and emotional aspects of the interaction are required.

## Create and start Docker containers

```bash
docker compose up -d
```

## Initiate and launch Docker containers individually

The MongoDB Docker image is needed for storing the data. You can download the image by running the following command.

```bash
docker pull mongo
```

The RESTART Rasa Duckling Docker image is needed for the natural language processing. You can download the image by running the following command.

```bash
docker pull rasa/duckling
```

The RESTART Rasa Docker image is needed for the natural language processing. You can build the image by running the following command.

```bash
docker build -t restart-rasa-actions ./language/actions
```

The Rasa Docker image is needed for the natural language processing. You can build the image by running the following command.

```bash
docker build -t restart-rasa ./language
```

Create the RESTART Docker image by running the following command.

```bash
docker build -t restart . --build-arg TRANSFORMER_HOST=restart-rasa --build-arg MONGO_HOST=restart-mongo
```

Run the RESTART MongoDB container by running the following command.

```bash
docker run -d --name restart-mongo -p 27017:27017 mongo
```

Run the RESTART Duckling container by running the following command.

```bash
docker run -d --name restart-duckling rasa/duckling
```

Run the RESTART Rasa Actions container by running the following command.

```bash
docker run -d --name restart-rasa-actions restart-rasa-actions
```

Run the RESTART Rasa container by running the following command.

```bash
docker run -d --name restart-rasa -p 5005:5005 restart-rasa
```

Run the RESTART container by running the following command.

```bash
docker run -d --name restart -p 8080:8080 restart
```