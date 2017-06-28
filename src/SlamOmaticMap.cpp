#include "SlamOmaticMap.h"


SlamOmaticMap::SlamOmaticMap(const unsigned int size, const uint8_t costMax, const uint8_t seuil, const double resMpPix)
  : IGeneric2dMap<uint8_t>(), m_first_update(true), m_seuil(seuil), m_maxcost(costMax), m_probability_map(new ProbabilityGridMap(size,resMpPix))
{
  //m_file = fopen("/home/vincent/Bureau/pose.csv","w");
  //m_cost_map = new CostMapV2(*m_probability_map,costMax,seuil);
  m_cost_map = new CostMap(*m_probability_map,costMax,seuil);
}

uint8_t SlamOmaticMap::at(const int x, const int y) const
{
  return m_probability_map->at(x,y);
}

uint8_t &SlamOmaticMap::at(const int x, const int y)
{
  return m_probability_map->at(x,y);
}

uint8_t SlamOmaticMap::at(const Pose2D &p) const
{
  return m_probability_map->at(p.x,p.y);
}

uint8_t &SlamOmaticMap::at(const Pose2D &p)
{
  return m_probability_map->at(p.x,p.y);
}

uint8_t SlamOmaticMap::at(const Point2D &p) const
{
  return m_probability_map->at(p.x,p.y);
}

uint8_t &SlamOmaticMap::at(const Point2D &p)
{
  return m_probability_map->at(p.x,p.y);
}

uint8_t SlamOmaticMap::costAt(const Pose2D &p) const
{
  return m_cost_map->at(p.x,p.y);
}

uint8_t SlamOmaticMap::costAt(const Point2D &p) const
{
  return m_cost_map->at(p.x,p.y);
}

uint8_t SlamOmaticMap::costMax() const
{
  return m_cost_map->max();
}

double SlamOmaticMap::resolution() const
{
  return m_probability_map->resolutionMpPix();
}

uint8_t SlamOmaticMap::costAt(const int x, const int y) const
{
  return m_cost_map->at(x,y);
}

void SlamOmaticMap::update(const Pose2D &p, const LiDAR_Scan_2D &scan)
{
  unsigned int i, size = scan.size();

  //fprintf(m_file,"%lu\t%f\t%f\t%f\r\n",ros::Time::now(),p.x,p.y,p.theta);
  //fflush(m_file);

  LiDAR_Scan_2D loc_scan_pix(scan);
  int pix_x = p.x / resolution();
  int pix_y = p.y / resolution();

  // ___________________________
  // ::: MaJ Probability Map :::
  double _cos = cos(p.theta), _sin = sin(p.theta);
  double dist = 0.0f, seuil = SEUIL_DIST_CARTO*SEUIL_DIST_CARTO;

  //double cost = 0.0f;

  for(i = 0; i < size; i++)
  {
    if(scan.m_points[i].x == 0 && scan.m_points[i].y == 0) continue;

    dist = scan.m_points[i].x * scan.m_points[i].x + scan.m_points[i].y * scan.m_points[i].y;
    if(dist > seuil) {
      loc_scan_pix.m_points[i].x = 0;
      loc_scan_pix.m_points[i].y = 0;
    } else {
      loc_scan_pix.m_points[i].x = (int)((_cos*scan.m_points[i].x - _sin*scan.m_points[i].y + p.x) / resolution());
      loc_scan_pix.m_points[i].y = (int)((_sin*scan.m_points[i].x + _cos*scan.m_points[i].y + p.y) / resolution());
    }

  }

  std::vector<Point2D>::iterator it;
  it = std::unique (loc_scan_pix.m_points.begin(), loc_scan_pix.m_points.end());
  loc_scan_pix.m_points.resize( std::distance(loc_scan_pix.m_points.begin(),it) );

  cout << "AVANT : " << scan.size() << " / APRES : " << loc_scan_pix.size() << endl;

  //cout << "Av1" << endl;
  m_probability_map->razChange();
  //cout << "Ap1" << endl;
  for(i = 0; i < loc_scan_pix.size(); i++)
    if(loc_scan_pix.m_points[i].x != 0 && loc_scan_pix.m_points[i].y != 0)
      //&& (m_first_update || costAt(loc_scan_pix.m_points[i].x,loc_scan_pix.m_points[i].y) <= 1))

      m_probability_map->pushLine(pix_x, pix_y, loc_scan_pix.m_points[i].x, loc_scan_pix.m_points[i].y,m_first_update);
  //at(loc_scan_pix.m_points[i].x,loc_scan_pix.m_points[i].y) == 50 );
  //  m );

  //  {
  //    ox = scan.m_points[i].x;
  //    oy = scan.m_points[i].y;

  //    if(ox == 0 && oy == 0) continue;

  //    x = _cos*ox - _sin*oy + p.x;
  //    y = _sin*ox + _cos*oy + p.y;

  //    m_probability_map->pushLine(p.x, p.y, x, y, m_first_update);
  //  }

  m_first_update = false;

  // ____________________
  // ::: MaJ Cost Map :::

  delete m_cost_map;
  m_cost_map = new CostMap(*m_probability_map,m_maxcost,m_seuil);
  //  cout << "Av2" << endl;
  //  m_cost_map->update(*m_probability_map);
  //  cout << "Ap2" << endl;

  //m_cost_map = new CostMap(*m_probability_map,m_maxcost,m_seuil);

  //exit(1);

  //    cv::Mat carte = cv::Mat(m_cost_map->size(), m_cost_map->size(),CV_8UC1,m_cost_map->data(), m_cost_map->size()).clone();
  //    cv::Mat carte2;
  //    carte.convertTo(carte2,CV_8UC1,255/m_cost_map->max());
  //cv::drawMarker(carte2,cv::Point(m_cost_map->size()/2,m_cost_map->size()/2),cv::Scalar(255));

  //  for(i = 0; i < size; i++)
  //  {
  //    ox = scan.m_points[i].x;
  //    oy = scan.m_points[i].y;

  //    if(ox == 0 && oy == 0) continue;

  //    x = _cos*ox - _sin*oy + p.x;
  //    y = _sin*ox + _cos*oy + p.y;

  //    int _x = x + m_cost_map->size()/2;
  //    int _y = -y + m_cost_map->size()/2;

  //    cost += m_cost_map->at(x,y);
  //    cv::drawMarker(carte2,cv::Point(_x,_y),cv::Scalar(128),cv::MARKER_TILTED_CROSS,3);
  //  }
  //  cout << "COST COST : " << (int)cost << " and by cell : " << (int)cost/scan.size() << endl;

  //    cv::imwrite("/home/vincent/Bureau/log/map.png",carte2);
}

nav_msgs::OccupancyGrid SlamOmaticMap::toRosOccupancyGrid() const
{
  nav_msgs::OccupancyGrid occupancy;
  occupancy.info.height = m_probability_map->size();
  occupancy.info.width = m_probability_map->size();
  occupancy.info.resolution = m_probability_map->resolutionMpPix();
  occupancy.info.origin.position.x = -(m_probability_map->size()*m_probability_map->resolutionMpPix())/2.0;
  occupancy.info.origin.position.y = -(m_probability_map->size()*m_probability_map->resolutionMpPix())/2.0;
  occupancy.info.origin.position.z = 0;
  occupancy.data.resize(m_probability_map->globalsize());
  //memcpy(occupancy.data.data(), m_cost_map->data(), m_probability_map->globalsize());
  memcpy(occupancy.data.data(), m_probability_map->data(), m_probability_map->globalsize());
  return occupancy;
}

bool SlamOmaticMap::save(const char *filename)
{
  //  cv::Mat prob, cost;
  //  std::string   str(filename), str_prob, str_cost;
  //  str_prob = str+"_prob.png";
  //  str_cost = str+"_cost.png";
  // Save ProbMap :
  //  prob = cv::Mat(m_probability_map->size(), m_probability_map->size(),CV_8UC1,m_probability_map->data(), m_probability_map->size()).clone();
  //  prob.convertTo(prob,CV_8UC1,2.55f);
  //  cv::imwrite(str_prob.c_str(),prob);
  // Save CostMap :
  //  cost = cv::Mat(m_cost_map->size(), m_cost_map->size(),CV_8UC1,m_cost_map->data(), m_cost_map->size()).clone();
  //  cost.convertTo(cost,CV_8UC1,255/m_cost_map->max());
  //  cv::imwrite(str_cost.c_str(),cost);
  return true;
}

bool SlamOmaticMap::load(const char *filename)
{
  //  cv::Mat prob, cost;
  //  int size;
  //  string str_prob, str_cost;
  //  str_prob = string(filename)+"_prob.png";
  //  str_cost = string(filename)+"_cost.png";
  //  //    ::: Load ProbMap :::
  //  try{
  ////    prob = cv::imread(str_prob.c_str(),cv::IMREAD_GRAYSCALE);
  ////    prob.convertTo(prob,CV_8UC1,1/2.55f);
  //    size = prob.total() * prob.elemSize();
  //    std::memcpy(m_probability_map->data(),prob.data,size);
  //  }catch(exception ex)
  //  {
  //    return false;
  //  }
  //  //    ::: Load CostMap :::
  //  try{
  ////    cost = cv::imread(str_cost.c_str(),cv::IMREAD_GRAYSCALE);
  ////    cost.convertTo(cost,CV_8UC1,1/2.55f);
  //    size = cost.total() * cost.elemSize();
  //    std::memcpy(m_cost_map->data(),cost.data,size);
  //  }catch(exception ex)
  //  {
  //    return false;
  //  }
  return true;
}
