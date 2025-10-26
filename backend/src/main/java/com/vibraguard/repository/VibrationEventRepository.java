package com.vibraguard.repository;

import com.vibraguard.model.VibrationEvent;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface VibrationEventRepository extends JpaRepository<VibrationEvent, Long> {

    List<VibrationEvent> findByDeviceIdOrderByEventTimestampDesc(Long deviceId);

    List<VibrationEvent> findTop100ByOrderByEventTimestampDesc();

}
