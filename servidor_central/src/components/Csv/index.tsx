import { CSVLink } from 'react-csv';
import { FiDownload } from 'react-icons/fi';

import Button from '../Button';

interface DataProps {
  date: string;
  device: string;
  event: string;
}

interface CsvProps {
  data: DataProps[];
}

const Csv = ({ data }: CsvProps) => {
  const headers = [
    { label: 'Data e Horário', key: 'date' },
    { label: 'Dispositivo', key: 'device' },
    { label: 'Evento', key: 'event' },
  ];

  return (
    <CSVLink
      data={data}
      headers={headers}
      filename="Logs - Central de Controle.csv"
    >
      <Button
        title="Download logs"
        icon={<FiDownload color="#fff" size={20} />}
      />
    </CSVLink>
  );
};

export default Csv;
