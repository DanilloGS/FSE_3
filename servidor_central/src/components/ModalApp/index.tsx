import { ReactNode } from 'react';
import './styles.scss';

import Modal from 'react-modal';

interface ModalProps {
  modalIsOpen: boolean;
  closeModal(): void;
  children: ReactNode;
}

const ModalApp = ({ modalIsOpen, closeModal, children }: ModalProps) => {
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
        {children}
      </div>
    </Modal>
  );
};

export default ModalApp;
