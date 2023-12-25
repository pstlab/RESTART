# use a python container as a starting point
FROM python:3.10-slim

# Expose the port that Rasa uses to run
EXPOSE 5005

# Install Rasa and spaCy
RUN pip3 install rasa spacy

# Install the Italian language model for spaCy
RUN python3 -m spacy download it_core_news_sm

# Copy the current directory contents into the container at /app
COPY . /app

# Set the working directory to /app
WORKDIR /app

# Train the model
RUN rasa train

# Make volume for models
VOLUME "/app/models"

# Run the app when the container launches
CMD [ "rasa", "run", "--enable-api", "--cors", "*", "--debug" ]