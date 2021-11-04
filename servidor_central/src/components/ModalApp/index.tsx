import './styles.scss';

import Modal from 'react-modal';
import { FiX } from 'react-icons/fi';

interface ModalProps {
  modalIsOpen: boolean;
  closeModal(): void;
}

const ModalApp = ({ modalIsOpen, closeModal }: ModalProps) => {
  return (
    <Modal
      isOpen={modalIsOpen}
      onRequestClose={closeModal}
      style={{
        overlay: {
          backgroundColor: 'rgba(0, 0, 0, 0.75)',
        },
        content: {
          width: '30%',
          top: '50%',
          left: '50%',
          right: 'auto',
          bottom: 'auto',
          marginRight: '-50%',
          transform: 'translate(-50%, -50%)',
          background: '#fff',
          overflow: 'auto',
          WebkitOverflowScrolling: 'touch',
          borderRadius: '4px',
          borderColor: '#fff',
          outline: 'none',
          padding: '2rem',
        },
      }}
      contentLabel="Escolha um opção"
      ariaHideApp={false}
    >
      <div id="modal-app">
        <header>
          <h2>Novo dispositivo</h2>
          <button onClick={closeModal}>
            <FiX color="#000" size={30} />
          </button>
        </header>

        <section>
          <div className="input-group">
            <h3>Cômodo do dispositivo</h3>
            <input />
          </div>
          <div className="input-group">
            <h3>Nome da entrada</h3>
            <input />
          </div>
          <div className="input-group">
            <h3>Nome da saída</h3>
            <input />
          </div>
        </section>

        <button>Adicionar</button>
      </div>
    </Modal>
  );
};

export default ModalApp;