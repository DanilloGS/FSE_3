FROM node:14

RUN npm install -g npm@latest

WORKDIR /central_server

COPY ["package*.json", "yarn.lock", "./"]

RUN npm install

COPY . .

CMD ["npm", "start"]
