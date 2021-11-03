FROM node:14-alpine

RUN npm install -g npm@latest

WORKDIR /app

COPY package.json ./
COPY yarn.lock ./

RUN npm install

COPY . .

EXPOSE 3000

CMD ["npm", "start"]
