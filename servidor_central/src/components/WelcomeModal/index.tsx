import './styles.scss';

import Modal from 'react-modal';

interface Props {
  modalIsOpen: boolean;
  onClick(): void;
}

const WelcomeModal = ({ modalIsOpen, onClick }: Props) => {
  return (
    <Modal
      isOpen={modalIsOpen}
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
      <div id="welcome-modal">
        <h2>Bem vindo à Central de Controle</h2>
        <p>Clique no botão abaixo para começar!</p>
        <button onClick={onClick}>COMEÇAR</button>
      </div>
    </Modal>
  );
};

export default WelcomeModal;
