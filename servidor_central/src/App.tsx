import { useState } from 'react';
import './App.scss';
import './global.scss';

import { FiPlus, FiDownload } from 'react-icons/fi';

import Button from './components/Button';
import ModalApp from './components/ModalApp';

function App() {
  const [modalIsOpen, setModalIsOpen] = useState(false);

  function handleModal() {
    setModalIsOpen(!modalIsOpen);
  }

  return (
    <div id="home">
      <ModalApp
        modalIsOpen={modalIsOpen}
        closeModal={() => setModalIsOpen(!modalIsOpen)}
      />
      <header>
        <h1>Central de Controle</h1>

        <section>
          <Button
            title="Adicionar dispositivo"
            icon={<FiPlus color="#fff" size={20} />}
            onClick={() => handleModal()}
          />
          <Button
            title="Download logs"
            icon={<FiDownload color="#fff" size={20} />}
          />
        </section>
      </header>
    </div>
  );
}

export default App;
