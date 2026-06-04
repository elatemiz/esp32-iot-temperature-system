<?php
$dataFile = 'data.json'; // Veri saklamak için JSON dosyası

// Yanıtın JSON formatında olduğunu belirt
header('Content-Type: application/json');

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Gelen verileri al
    $temperature = filter_input(INPUT_POST, 'temperature', FILTER_SANITIZE_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION);
    $humidity = filter_input(INPUT_POST, 'humidity', FILTER_SANITIZE_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION);

    if ($temperature !== null && $humidity !== null) {
        // Veriyi JSON dosyasına kaydet
        $data = ['temperature' => $temperature, 'humidity' => $humidity];
        $result = file_put_contents($dataFile, json_encode($data, JSON_PRETTY_PRINT), LOCK_EX);

        if ($result === false) {
            echo json_encode(['status' => 'error', 'message' => 'Failed to save data']);
        } else {
            // Başarılı yanıt gönder
            echo json_encode(['status' => 'success', 'message' => 'Data saved']);
        }
    } else {
        echo json_encode(['status' => 'error', 'message' => 'Invalid or missing data']);
    }
} elseif ($_SERVER['REQUEST_METHOD'] === 'GET') {
    // JSON dosyasından veri oku
    if (file_exists($dataFile)) {
        $data = file_get_contents($dataFile);
        if ($data === false) {
            echo json_encode(['status' => 'error', 'message' => 'Failed to read data']);
        } else {
            echo $data;
        }
    } else {
        echo json_encode(['status' => 'error', 'message' => 'No data found']);
    }
} else {
    echo json_encode(['status' => 'error', 'message' => 'Invalid request method']);
}
?>

